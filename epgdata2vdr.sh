#!/bin/bash

. /etc/vdr/epgdata2vdr.conf

# only change if you know what you are doing
EPGDATA2VDRBIN=/usr/bin/epgdata2vdr
SVDRPSENDBIN=/usr/bin/svdrpsend
CURLBIN=/usr/bin/curl
UNZIPBIN=/usr/bin/unzip

# install required files if necessary + create directories
if [ ! -n "$WORKDIR" ]; then 
   echo "Work directory undefined. Stopping."
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
    if [ -n "$PIN" ]; then 
      $CURLBIN "http://www.epgdata.com/index.php?action=sendInclude&iOEM=&pin=$PIN&dataType=xml" \
           -o $WORKDIR/include/include.zip
      $UNZIPBIN -o $WORKDIR/include/include.zip -d $WORKDIR/include
    else
      echo "epgdata.com PIN is not configured"
      exit 1
    fi
   fi
fi

if [ -n "$PIN" ]; then
for i in `seq 0 $MAXDAYS` ; do
  TMP=`mktemp`
  $CURLBIN -I -D $TMP "http://www.epgdata.com/index.php?action=sendPackage&iOEM=VDR&pin=$PIN&dayOffset=$i&dataType=xml" &> /dev/null
  FILE=`grep -e "^Content-disposition.*$" $TMP | sed -e ' s/\r//g' | cut -d"=" -f2`
  FILE="`basename $FILE .zip`"
  SIZE=`grep -e "^Content-Length.*$" $TMP  | sed -e ' s/\r//g' | cut -d":" -f2`
  if [ ! -z $SIZE ]; then
    if [ ! -e $WORKDIR/files/$FILE.zip ]; then
      $CURLBIN "http://www.epgdata.com/index.php?action=sendPackage&iOEM=VDR&pin=$PIN&dayOffset=$i&dataType=xml" -o $WORKDIR/files/$FILE.zip
      rm $WORKDIR/files/$FILE.epg > /dev/null 2>&1
    else
      if [ `ls -la $WORKDIR/files/$FILE.zip | cut -d" " -f5` != $SIZE  ]; then
        nice -n 19 $CURLBIN "http://www.epgdata.com/index.php?action=sendPackage&iOEM=VDR&pin=$PIN&dayOffset=$i&dataType=xml" -o $WORKDIR/files/$FILE.zip
        rm $WORKDIR/files/$FILE.epg > /dev/null 2>&1
      else
        echo "File: $FILE already downloaded"
      fi
    fi
### process  start ###
    if [ -s $WORKDIR/files/$FILE.epg ]; then
      echo "File: $FILE already processed"
    else
      echo -e " File: $FILE  Size: $(( $SIZE /1024 )) kB"
      # With EPG-Images
      #epgdata2vdr          includedir        epgimagesdir file(s)
      $EPGDATA2VDRBIN $WORKDIR/include/ $EPGIMAGES $WORKDIR/files/$FILE.zip 
      $SVDRPSENDBIN PUTE ${PUTECHAR}$WORKDIR/files/$FILE.epg
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

else
  echo "epgdata.com PIN is not configured"
  exit 1
fi

# Delete old EPG-Images
if [ -e $WORKDIR/files/images/ ]; then 
   find $WORKDIR/files/images/* -type f -mtime +$MAXDAYS -print0 | xargs -0 rm -f
fi 
# Also delete old symlinks (-L only broken symlinks)
[ -n "$EPGIMAGES" ] && find -L $EPGIMAGES/* -type l -delete &> /dev/null
