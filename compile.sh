NAME=$1
if [ "$NAME" = "" ]; then
    NAME="out"
fi
g++ *.cpp Engine2D/*.cpp -o $NAME -lglfw -lpng -lGL -lGLEW -lalut -lopenal -laudio
