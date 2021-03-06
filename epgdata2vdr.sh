#!/bin/bash

. /etc/vdr/epgdata2vdr.conf

# only change if you know what you are doing
EPGDATA2VDRBIN=/usr/bin/epgdata2vdr
SVDRPSENDBIN=/usr/bin/svdrpsend
CURLBIN=/usr/bin/curl
UNZIPBIN=/usr/bin/unzip
SENDMAIL=/usr/sbin/sendmail
WC=/bin/wc

# install required files if necessary + create directories
if [ ! -n "$WORKDIR" ]; then
	echo "Work directory undefined. Stopping."
	exit 1
fi

if [ ! -n "$PIN" ]; then
	echo "epgdata.com PIN is not configured. Stopping"
	exit 1
fi

if [ ! -e $CONF ]; then
	echo "$CONF not found. Stopping."
	exit 1
fi

if [ ! -d $WORKDIR/files ]; then
	mkdir -p $WORKDIR/files
fi

if [ ! -e $INCLUDEDIR/genre.xml -o ! -e $INCLUDEDIR/category.xml ]; then
	mkdir -p $INCLUDEDIR
	if [ x$? != x0 ]; then
		echo "$INCLUDEDIR exists but is not a directory"
		exit 1
	else
		nice -n 19 $CURLBIN "http://www.epgdata.com/index.php?action=sendInclude&iOEM=&pin=$PIN&dataType=xml" \
		-o $INCLUDEDIR/include.zip
		$UNZIPBIN -o $INCLUDEDIR/include.zip -d $INCLUDEDIR
	fi
fi

# Delete old EPG-Images
if [ -e $WORKDIR/images/ ]; then
	find $WORKDIR/images/* -type f -mtime +$MAXDAYS -print0 | xargs -0 rm -f
fi
# Also delete old symlinks (-L only broken symlinks)
if [ -n "$EPGIMAGES" ]; then 
   find -L $EPGIMAGES/* -type l -print0 | xargs -0 rm -f
   IMAGEOPT="-i $EPGIMAGES/"
fi

# process data
for i in `seq 0 $MAXDAYS` ; do
	echo "<--- Processing data with offset $i --->"
	TMP=`mktemp`
	nice -n 19 $CURLBIN -I -D $TMP "http://www.epgdata.com/index.php?action=sendPackage&iOEM=VDR&pin=$PIN&dayOffset=$i&dataType=xml" &> /dev/null
	FILE=`grep -e "^Content-disposition.*$" $TMP | sed -e ' s/\r//g' | cut -d"=" -f2`
	FILE="`basename $FILE .zip`"
	SIZE=`grep -e "^Content-Length.*$" $TMP | sed -e ' s/\r//g' | cut -d":" -f2`
	[ -z $TIMEOUT ] && TIMEOUT=`grep -e "^x-epgdata-timeout.*$" $TMP | sed -e ' s/\r//g' | cut -d":" -f2`
	if [ -z $LEFT ] && [ -n "$TIMEOUT" ]; then
		LEFT=$((( $TIMEOUT - $(date +%s)) / 60 / 60 / 24 ))
	fi
	if [ ! -z $SIZE ]; then
		if [ ! -e $WORKDIR/files/$FILE.zip ]; then
			nice -n 19 $CURLBIN "http://www.epgdata.com/index.php?action=sendPackage&iOEM=VDR&pin=$PIN&dayOffset=$i&dataType=xml" -o $WORKDIR/files/$FILE.zip
			rm $WORKDIR/$FILE.epg > /dev/null 2>&1
			else
			if [ `ls -la $WORKDIR/files/$FILE.zip | cut -d" " -f5` != $SIZE  ]; then
				nice -n 19 $CURLBIN "http://www.epgdata.com/index.php?action=sendPackage&iOEM=VDR&pin=$PIN&dayOffset=$i&dataType=xml" -o $WORKDIR/files/$FILE.zip
				rm $WORKDIR/$FILE.epg > /dev/null 2>&1
			else
				echo "File: $FILE already downloaded"
			fi
		fi
		### process  start ###
		if [ -s $WORKDIR/$FILE.epg ]; then
			echo "File: $FILE already processed"
		else
			if [ -e $WORKDIR/files/$FILE.zip ]; then
				echo -e " File: $FILE  Size: $(( $SIZE / 1024 )) kB"

				#epgdata2vdr --help for explanation
				$EPGDATA2VDRBIN -c $CONF \
                                                -I $WORKDIR/include/ \
                                                   $IMAGEOPT \
                                                -p ${WORKDIR}/ \
                                                -f $IMAGE_FORMAT \
                                                -s $IMAGE_MAXSIZE \
                                                   $WORKDIR/files/$FILE.zip
				$SVDRPSENDBIN PUTE ${PUTECHAR}$WORKDIR/$FILE.epg
		else
			echo "File: Failed to load $FILE"
			fi
		fi
	fi
	rm $TMP
done

# Delete all but last set
echo -n "Cleanup old files ... "
for i in `find $WORKDIR/files/* -name "*$SUFFIX.zip" | cut -d"_" -f2 | sort -r | uniq | tail -n +2` ; do
	echo "Cleanup files of : $i "
	rm -f $WORKDIR/*$i$SUFFIX.epg
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
			echo "" >> /tmp/mail.txt
			echo "Das Abo bei EPGData.com hat noch eine Laufzeit von $LEFT Tag(en)" >> /tmp/mail.txt
			echo "und endet danach automatisch! Ein neues Abo kann unter" >> /tmp/mail.txt
			echo "http://www.epgdata.com/index.php?action=newSubscription&iLang=de&iOEM=vdr&iCountry=de&popup=0" >> /tmp/mail.txt
			echo "abgeschlossen werden. Das neue Abo startet sofort nach Bezahlung!" >> /tmp/mail.txt
		else
			echo "Subject: Fehler beim laden von EPGData.com!" >> /tmp/mail.txt
			echo "" >> /tmp/mail.txt
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
