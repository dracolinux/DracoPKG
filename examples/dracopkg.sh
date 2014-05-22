#!/bin/sh

#dbus-send --system --type=method_call --dest='org.dracolinux.DracoPKG' /pkgsrc 'org.dracolinux.DracoPKG.PkgSrc.initPkgsrc'
#qdbus --system org.dracolinux.DracoPKG /pkgsrc initPkgsrc

echo "===> Init pkgsrc ..."
qdbus --system org.dracolinux.DracoPKG /pkgsrc initPkgsrc | dbus-monitor --system type=signal, sender=org.dracolinux.DracoPKG, interface=org.dracolinux.DracoPKG.PkgSrc, path=/pkgsrc | \
while read line; do
  BOOTSTRAP_STATUS=$(echo $line | sed 's/.*member=bootstrapStatus//;/===>/!d;s/string "//g;')
  if [ "$BOOTSTRAP_STATUS" ]; then
    echo $BOOTSTRAP_STATUS
  fi
  FAIL=$(echo $line | sed '/member=pkgsrcFailed/!d')
  DONE=$(echo $line | sed '/member=pkgsrcReady/!d')
  if [ "$DONE" ] || [ "$FAIL" ]; then
    if [ "$FAIL" ]; then
      echo "===> Failed to init pkgsrc"
    fi
    PID=$(pgrep -P $$ dbus-monitor)
    kill $PID
  fi
done

if [ "$1" ]; then
  PKGS=$(qdbus --system org.dracolinux.DracoPKG /pkgsrc packageList "" "$1")
fi

if [ "$PKGS" ]; then
#echo "===> Found the following packages:"
echo $PKGS | sed 's/|/ /g'
echo -n "===> Install? y/N "
read answer

if [ "$answer" = "y" ];then
echo ok

for i in $PKGS; do

PKG=$(echo $i | sed 's/|/ /g' | awk '{print $2}')
CAT=$(echo $i | sed 's/|/ /g' | awk '{print $1}')
qdbus --system org.dracolinux.DracoPKG /pkgsrc bmakeStart "$PKG" "$CAT" "" "install"
dbus-monitor --system type=signal, sender=org.dracolinux.DracoPKG, interface=org.dracolinux.DracoPKG.PkgSrc, path=/pkgsrc | \
while read line; do
  STATUS=$(echo $line | sed 's/.*member=bmakeStatus//;/===>/!d;s/string "//g;')
  if [ "$STATUS" ]; then
    echo $STATUS
  fi
  DONE=$(echo $line | sed '/member=bmakeFinished/!d')
  if [ "$DONE" ]; then
    PID=$(pgrep -P $$ dbus-monitor)
    kill $PID
  fi
done

done

fi
fi

