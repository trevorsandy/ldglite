#!/bin/sh

ME="$(basename "$(test -L "$0" && readlink "$0" || echo "$0")")"
WORK_DIR=`pwd`

# logging stuff
LOG="${PWD}/../ldglite/$ME.log"
if [ -f ${LOG} -a -r ${LOG} ]
then
        rm ${LOG}
fi
touch ${LOG}

exec > >(tee -a ${LOG} )
exec 2> >(tee -a ${LOG} >&2)

echo "Start $ME execution..."
if [ "$1" = "" ]
then
        echo "Warning: Did not receive VERSION INFO."
        echo "Using Default Version: 1.3.1"
        VERSION="1.3.2"
else
        echo "1. capture version info - using $1..."
        VERSION=$1
fi

echo "2. create bundle directory structure..."
rm -rf ldglite.app
mkdir -p ldglite.app/Contents/MacOS
mkdir -p ldglite.app/Contents/Resources

echo "3. write Info.plist..."
cat <<END > ldglite.app/Contents/Info.plist
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist SYSTEM "file://localhost/System/Library/DTDs/PropertyList.dtd">
<plist version="0.9">
  <dict>
    <key>CFBundleDevelopmentRegion</key>    <string>English</string>
    <key>CFBundleInfoDictionaryVersion</key>    <string>6.0</string>
    <key>CFBundleExecutable</key>       <string>ldglite</string>
    <key>CFBundlePackageType</key>      <string>APPL</string>
    <key>CFBundleSignature</key>        <string>LdGL</string>
    <key>CFBundleName</key>         <string>ldglite</string>
    <key>CFBundleVersion</key>          <string>${VERSION}</string>
    <key>CFBundleShortVersionString</key>   <string>ldglite ${VERSION}</string>
    <key>CFBundleGetInfoString</key>        <string>ldglite ${VERSION} https://github.com/trevorsandy/ldglite</string>
    <key>CFBundleIconFile</key>         <string>ldglite.icns</string>
    <key>CFBundleIdentifier</key>       <string>org.ldraw.ldglite</string>
    <key>CSResourcesFileMapped</key>        <true/>
    <key>CFBundleDocumentTypes</key>
    <array>
      <dict>
    <key>CFBundleTypeExtensions</key>
          <array>
        <string>LDR</string>
        <string>ldr</string>
        <string>MPD</string>
        <string>mpd</string>
        <string>DAT</string>
        <string>dat</string>
          </array>
        <key>CFBundleTypeIconFile</key>     <string>ldraw_document.icns</string>
    <key>CFBundleTypeName</key>     <string>Ldraw Document</string>
    <key>CFBundleTypeMIMETypes</key>
      <array>
        <string>application/x-ldraw</string>
      </array>
        <key>CFBundleTypeOSTypes</key>      <array><string>LDR</string></array>
    <key>CFBundleTypeRole</key>     <string>Viewer</string>
    <key>NSDocumentClass</key>
      <string>BrowserDocument</string>
      </dict>
    </array>
  </dict>
</plist>
END
echo "4. write PkgInfo..."
echo "APPLLdGL" > ldglite.app/Contents/PkgInfo

echo "5. move executable, wrapper and icon to bundle..."
EXE="ldglite"
ICON="ldglite.icns"
WRAPPER="ldglite_w.command"
cp ${EXE} ldglite.app/Contents/MacOS
cp ${ICON} ldglite.app/Contents/Resources
cp ${WRAPPER} ldglite.app/Contents/MacOS

echo "6. update wrapper permissions..."
chmod 755 ldglite.app/Contents/MacOS/${WRAPPER}

echo "Script $ME execution finshed."
