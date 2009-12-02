#!/bin/bash

. /etc/vdr/addons/epgdata2vdr.conf

# install required files if necessary
if [ ! -d $WORKDIR/include ]; then 
   mkdir -p $WORKDIR/include 
   if [ x$? != x0 ]; then 
      echo "$WORKDIR/include exists but is not a directory"
      exit 1
   else 
      curl "http://www.epgdata.com/index.php?action=sendInclude&iOEM=&pin=$PIN&dataType=xml" \
           -o $WORKDIR/include/include.zip
      unzip -o $WORKDIR/include/include.zip -d $WORKDIR/include
fi 

for i in `seq 0 $MAXDAYS` ; do
  TMP=`mktemp`
  nice -19 curl -I -D $TMP "http://www.epgdata.com/index.php?action=sendPackage&iOEM=VDR&pin=$PIN&dayOffset=$i&dataType=xml" &> /dev/null
  FILE=`grep -e "^Content-disposition.*$" $TMP | sed -e ' s/\r//g' | cut -d"=" -f2`
  FILE="`basename $FILE .zip`"
  SIZE=`grep -e "^Content-Length.*$" $TMP  | sed -e ' s/\r//g' | cut -d":" -f2`
  if [ ! -z $SIZE ]; then
    if [ ! -e $WORKDIR/files/$FILE.zip ]; then
      nice -19 curl "http://www.epgdata.com/index.php?action=sendPackage&iOEM=VDR&pin=$PIN&dayOffset=$i&dataType=xml" -o $WORKDIR/files/$FILE.zip
      rm $WORKDIR/files/$FILE.epg > /dev/null 2>&1
    else
      if [ `ls -la $WORKDIR/files/$FILE.zip | cut -d" " -f5` != $SIZE  ]; then
        nice -19 curl "http://www.epgdata.com/index.php?action=sendPackage&iOEM=VDR&pin=$PIN&dayOffset=$i&dataType=xml" -o $WORKDIR/files/$FILE.zip
        rm $WORKDIR/files/$FILE.epg > /dev/null 2>&1
      else
        echo "File: $FILE already downloaded"
      fi
    fi
### process  start ###
    if [ -e $WORKDIR/files/$FILE.epg ]; then
      echo "File: $FILE already processed"
    else
      unzip -o $WORKDIR/files/$FILE.zip *.dtd -d $WORKDIR/include
      echo -e " File: $FILE  Size: $(( $SIZE /1024 )) kB"
      LANG=de_DE.utf8 nice -19 epgdata2vdr $WORKDIR/include/ $WORKDIR/files/$FILE.zip > $WORKDIR/files/$FILE.epg
      svdrpsend PUTE ${PUTECHAR}$WORKDIR/files/$FILE.epg
    fi
  fi
  rm $TMP
done

echo -n "Cleanup old files ... "
for i in `find $WORKDIR/files/* -name "*$SUFFIX.zip" | cut -d"_" -f2 | sort -r | uniq | tail  -n +2` ; do
 echo "Cleanup files of : $i "
 rm -f $WORKDIR/files/*$i$SUFFIX.epg
 rm -f $WORKDIR/files/*$i$SUFFIX.zip
done

# Delete old EPG-Images
find $WORKDIR/files/images/* -type f -mtime +$MAXDAYS -print0 | xargs -0 rm -f
# Also delete old symlinks (-L only broken symlinks)
[ -n $EPGIMAGES ] && find -L $EPGIMAGES/* -type l -delete

