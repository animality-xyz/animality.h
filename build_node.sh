printf "animality: preparing..."

ANIMALITY_ROOT=$(pwd)
ANIMALITY_DEPS=$(echo $ANIMALITY_ROOT"/deps/")

if test -f $ANIMALITY_DEPS"libanimal.a"; then
    printf " failed.\nerror: already built.\n"
    exit 1
fi

mkdir deps && cd ..

printf "\nanimality: installing 'libcurl'..."
apt-get install libcurl4-openssl-dev > /dev/null

printf " done.\nanimality: installing 'cJSON'..."
git clone --quiet https://github.com/DaveGamble/cJSON.git > /dev/null
cd $ANIMALITY_ROOT

printf " done.\nanimality: installing 'node-gyp'...\n"
npm i -g node-gyp

printf "animality: installing 'node-addon-api'...\n"
npm init -y
npm i node-addon-api

cd .. && printf "animality: building dependency \"cJSON\"..."
cd cJSON/

gcc -c cJSON.c -o cJSON.o
ar rcs -o libjson.a -c cJSON.o
mv libjson.a cJSON.h $ANIMALITY_DEPS

cd .. && rm -rf cJSON/

printf " done.\nanimality: copying node api bindings..."
cd $ANIMALITY_ROOT

mv ./node_modules/node-addon-api/*.h ./deps/
rm -rf ./node_modules/ && rm *.json

printf " done.\nanimality: building main C source..."
gcc -c animality.c -D _AN_NODE_ADDON -o animality.o
ar rcs -o libanimal.a animality.o
rm animality.o
mv libanimal.a $ANIMALITY_DEPS

printf " done.\nanimality: removing 'node-addon-api'...\n"
npm uninstall node-addon-api

printf "animality: building the node addon...\n"
node-gyp configure
node-gyp build

printf "\n\nanimality: node addon is built. congrats!\n"