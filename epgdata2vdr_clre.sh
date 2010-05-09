#!/bin/bash

# Einstellungen
. /etc/vdr/epgdata2vdr/epgdata2vdr.conf

# Debugging
#set -x

# Spracheinstellungen, damit Umlaute zum VDR passen,
# wenn dieser in einer anderen Einstellung läuft (z.B. wegen dem Sky-EPG)
export LANG=de_DE
export LC_COLLATE=de_DE

[ -n "$1" ] && MAXDAYS=$1   # Anzahl der Tage kann auch manuell übergeben werden

# Pfade zu den Programmen
CURL=/usr/bin/curl
EPGDATA2VDR=/usr/local/src/_div/epgdata2vdr/epgdata2vdr
SVDRPSEND=/usr/bin/svdrpsend.pl
MKDIR=/bin/mkdir
UNZIP=/usr/bin/unzip
SED=/bin/sed
SENDMAIL=/usr/sbin/sendmail
WC=/bin/wc

# check config and install required files if necessary
if [ ! -n "$WORKDIR" ]; then
	echo "Work directory undefined. Stopping."
	exit 1
fi

if [ ! -n "$PIN" ]; then
	echo "epgdata.com PIN is not configured"
	exit 1
fi

if [ ! -e $WORKDIR/include/epgdata2vdr_channelmap.conf ]; then
	echo "epgdata2vdr_channelmap.conf not found. Stopping."
	exit 1
fi

if [ ! -d $WORKDIR/files ]; then
	mkdir -p $WORKDIR/files
fi

if [ ! -e $WORKDIR/include/genre.xml -o ! -e $WORKDIR/include/category.xml ]; then
	mkdir -p $WORKDIR/include
	if [ x$? != x0 ]; then
		echo "$WORKDIR/include exists but is not a directory"
		exit 1
	else
		nice -n 19 $CURL "http://www.epgdata.com/index.php?action=sendInclude&iOEM=&pin=$PIN&dataType=xml" \
		-o $WORKDIR/include/include.zip
		$UNZIP -o $WORKDIR/include/include.zip -d $WORKDIR/include
	fi
fi

# Delete old EPG-Images
if [ -e $WORKDIR/files/images/ ]; then
	find $WORKDIR/files/images/* -type f -mtime +$MAXDAYS -print0 | xargs -0 rm -f
fi
# Also delete old symlinks (-L only broken symlinks)
[ -n "$EPGIMAGES" ] && find -L $EPGIMAGES/* -type l -print0 | xargs -0 rm -f

# Put configured channels in array (first element is 0)
EPGDATA_CHANNELS=( $(grep '^[0-9]* = S.*//' $WORKDIR/include/epgdata2vdr_channelmap.conf | cut -d "$(echo -e "\t")" -f1 | sed 's/[0-9]* = //' | sed "s/,/ /g" | awk '{printf "%s ",$0;}') )
#echo ${EPGDATA_CHANNELS[*]}
echo "${#EPGDATA_CHANNELS[*]} channels configured for EPGData2VDR"

# Download and process files for EPG
for i in `seq 0 $MAXDAYS` ; do
	echo "<--- Processing data with offset $i --->"
	TMP=`mktemp`
	nice -n 19 $CURL -I -D $TMP "http://www.epgdata.com/index.php?action=sendPackage&iOEM=VDR&pin=$PIN&dayOffset=$i&dataType=xml" &> /dev/null
	FILE=`grep -e "^Content-disposition.*$" $TMP | $SED -e ' s/\r//g' | cut -d"=" -f2`
	[ -z $FILE ] && echo "No Datafile found in Download"
	FILE="`basename $FILE .zip`"
	SIZE=`grep -e "^Content-Length.*$" $TMP | $SED -e ' s/\r//g' | cut -d":" -f2`
	#AVAILABLE=`grep -e "^x-epgdata-packageAvailable.*$" $TMP | sed -e ' s/\r//g' | cut -d":" -f2`
	[ -z $TIMEOUT ] && TIMEOUT=`grep -e "^x-epgdata-timeout.*$" $TMP | $SED -e ' s/\r//g' | cut -d":" -f2`
	if [ -z $LEFT ] && [ -n "$TIMEOUT" ]; then
		LEFT=$((( $TIMEOUT - $(date +%s)) / 60 / 60 / 24 )) # Days left (subscription)
	fi
	if [ ! -z $SIZE ]; then
		if [ ! -e $WORKDIR/files/$FILE.zip ]; then
			nice -n 19 $CURL "http://www.epgdata.com/index.php?action=sendPackage&iOEM=VDR&pin=$PIN&dayOffset=$i&dataType=xml" -o $WORKDIR/files/$FILE.zip #-v
			rm $WORKDIR/files/$FILE.epg > /dev/null 2>&1
		else
			if [ `ls -la $WORKDIR/files/$FILE.zip | cut -d" " -f5` != $SIZE  ]; then
				nice -n 19 $CURL "http://www.epgdata.com/index.php?action=sendPackage&iOEM=VDR&pin=$PIN&dayOffset=$i&dataType=xml" -o $WORKDIR/files/$FILE.zip #-v
				rm $WORKDIR/files/$FILE.epg > /dev/null 2>&1
			else
				echo "File: $FILE already downloaded"
			fi
		fi
		### process  start ###
		if [ -s $WORKDIR/files/$FILE.epg ]; then
			echo "File: $FILE already processed"
			NO_CLRE=1 # Do not delete EPG from VDR
		else
			if [ -e $WORKDIR/files/$FILE.zip ]; then
				echo -e " File: $FILE  Size: $(( $SIZE / 1024 )) kB"
				#epgdata2vdr includedir epgimagesdir file(s)
				nice -n 19 $EPGDATA2VDR $WORKDIR/include/ $EPGIMAGES $WORKDIR/files/$FILE.zip #> $WORKDIR/files/$FILE.epg
				nice -n 19 $SED -i 's/\x97/-/g' $WORKDIR/files/$FILE.epg # Replace long "-"
				# List of channels with data in epgfile
				EPG_CHANNELS=( $(grep '^C *' $WORKDIR/files/$FILE.epg | sed 's/C //' | awk '{printf "%s ",$0;}') )
				#echo ${EPG_CHANNELS[*]}
				echo "${#EPG_CHANNELS[*]} EPG entries found"
				# Check if data exist for each configured channel
				for chan in `seq 0 $(( ${#EPGDATA_CHANNELS[*]} -1 ))` ; do
					if [ "${EPGDATA_CHANNELS[$chan]}" != "0" ] ; then # skip already failed channels
						echo -n "Checking EPG for channel #$chan: "; echo ${EPGDATA_CHANNELS[$chan]}
						### Found?
						NUM[$chan]=0 # Set to 0 to avoid empty fields
						for entry in ${EPG_CHANNELS[*]} ; do # Is there some epgdata to import?
							if [ $entry == ${EPGDATA_CHANNELS[$chan]} ] ; then
								NUM[$chan]=$(( ${NUM[$chan]} +1 )) # Put in array (and count)
							fi
						done #entry
						if [ "${NUM[$chan]}" = "0" ]; then
							echo "!!> NO EPG ENTRIES FOUND!"
							# Do not delete EPG from VDR's EPG.data!
							EPGDATA_CHANNELS[$chan]=0
						else
							echo "--> ${NUM[$chan]} EPG entries found"
						fi
					fi
				done #chan
				echo "==> ${NUM[*]} EPG entries found"
				# Add epgfile to importlist (Will be processed later)
				IMPORT_LIST[$i]=$FILE
			else
				echo "File: Failed to load $FILE"
				NO_CLRE=1 # Do not delete EPG from VDR
			fi
		fi
	fi
	rm $TMP
done #i

#echo "${#EPGDATA_CHANNELS[*]} channels configured for EPGData2VDR:"
#echo ${EPGDATA_CHANNELS[*]}

# Delete old EPG from VDR's EPG.data
if [ -z $NO_CLRE ] ; then # Already procesd files or download error
	for i in ${EPGDATA_CHANNELS[*]} ; do
		if [ "$i" != "0" ] ; then
			$SVDRPSEND CLRE $i # Delete EPG for channel
		fi
	done
fi

# Import EPG
for i in ${IMPORT_LIST[*]} ; do
	$SVDRPSEND PUTE $PUTECHAR$WORKDIR/files/$i.epg # Import EPG to VDR
done

# Delete all but last set
for i in `find $WORKDIR/files/* -name "*$SUFFIX.zip" | cut -d"_" -f2 | sort -r | uniq | tail -n +2` ; do
	echo "Cleanup files of : $i "
	rm -f $WORKDIR/files/*$i$SUFFIX.epg
	rm -f $WORKDIR/files/*$i$SUFFIX.zip
done

if [ -n "$EMAIL" ]; then # To enable set $EMAIL in epgdata2vdr.conf
	# Check if all files could be loaded (only works if MAXDAYS is less than 14)
	NUMFILES=$(($(ls $WORKDIR/files/*.zip | wc -l)))
	if [ $NUMFILES -lt $(($MAXDAYS +1)) ] && [ $MAXDAYS -lt 14 ] || [ "$LEFT" -lt "5" ]; then
		echo "<--- Sending email about missing files / ending subscription --->"
		echo "From: \"EPGData2VDR-Skript\"<$EMAIL>" > /tmp/mail.txt
		echo "T0: $EMAIL" >> /tmp/mail.txt
		if [ "$LEFT" -lt "5" ]; then # Subscription is ending...
			echo "Subject: EPGData.com Abo endet in $LEFT Tag(en)!" >> /tmp/mail.txt
			echo  "" >> /tmp/mail.txt
			echo "Das Abo bei EPGData.com hat noch eine Laufzeit von $LEFT Tag(en)" >> /tmp/mail.txt
			echo "und endet danach automatisch! Ein neues Abo kann unter" >> /tmp/mail.txt
			echo "http://www.epgdata.com/index.php?action=newSubscription&iLang=de&iOEM=vdr&iCountry=de&popup=0" >> /tmp/mail.txt
			echo "abgeschlossen werden. Das neue Abo startet sofort nach Bezahlung!" >> /tmp/mail.txt
		else
			echo "Subject: Fehler beim laden von EPGData.com!" >> /tmp/mail.txt
			echo  "" >> /tmp/mail.txt
			echo "Beim Download von EPGData.com konnten nicht alle Daten" >> /tmp/mail.txt
			echo "geladen werden. Hinweis: Das Abo endet in $LEFT Tag(en)" >> /tmp/mail.txt
			echo "" >> /tmp/mail.txt
			echo "Es wurde(n) $NUMFILES von $(($MAXDAYS +1)) Datei(en) geladen!" >> /tmp/mail.txt
		fi
		echo "" >> /tmp/mail.txt
		echo "Inhalt von $WORKDIR/files:" >> /tmp/mail.txt
		ls -l $WORKDIR/files >> /tmp/mail.txt
		$SENDMAIL $EMAIL < /tmp/mail.txt
	fi
fi

exit
