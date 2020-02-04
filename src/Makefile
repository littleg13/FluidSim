CPP = g++ -std=c++11
INC = -I../cryphutil -I../fontutil -I../glslutil -I../ImageReader -I../mvcutil -I/usr/local/include -I.

# >>> FOR LINUX, uncomment next few lines; comment out the MAC ones.
C_FLAGS = -fPIC -g -c $(INC) -DGL_GLEXT_PROTOTYPES -pedantic
GL_LIB_LOC = -L/usr/lib/nvidia-375
GL_LIBRARIES = $(GL_LIB_LOC) -lglfw -lGLU -lGL
MAKE = make
# >>> FOR MAC, uncomment next few lines; comment out previous linux ones.
# C_FLAGS = -fPIC -g -c $(INC) -DGLFW_INCLUDE_GLEXT -DGLFW_INCLUDE_GLCOREARB -DGL_SILENCE_DEPRECATION
# GL_LIBRARIES = -L/usr/local/lib -lglfw -framework OpenGL
# MAKE = make -f MakefileMac
# >>> END: FOR LINUX - FOR MAC

LINK = g++ -fPIC -g
LOCAL_UTIL_LIBRARIES = ../lib/libcryph.so ../lib/libfont.so ../lib/libglsl.so ../lib/libImageReader.so ../lib/libmvc.so

OBJS = main.o ExtendedController.o PhongMaterial.o SceneElement.o Prism.o Cylinder.o Ground.o SwingSet.o LampPost.o BenchArm.o BenchChair.o Bench.o Path.o Snowman.o ParticleSystem.o

main: $(OBJS) $(LOCAL_UTIL_LIBRARIES)
	$(LINK) -o main $(OBJS) $(LOCAL_UTIL_LIBRARIES) $(GL_LIBRARIES)

../lib/libcryph.so: ../cryphutil/AffVector.h ../cryphutil/AffVector.c++ ../cryphutil/AffPoint.h ../cryphutil/AffPoint.c++
	(cd ../cryphutil; make)

../lib/libfont.so: ../fontutil/CFont.h ../fontutil/CFont.c++ ../fontutil/CGLString.h ../fontutil/CGLString.c++
	(cd ../fontutil; $(MAKE))

../lib/libglsl.so: ../glslutil/ShaderIF.h ../glslutil/ShaderIF.c++
	(cd ../glslutil; $(MAKE))

../lib/libImageReader.so: ../ImageReader/ImageReader.h ../ImageReader/ImageReader.c++
	(cd ../ImageReader; $(MAKE))

../lib/libmvc.so: ../mvcutil/Controller.h ../mvcutil/Controller.c++ ../mvcutil/ModelView.h ../mvcutil/ModelView.c++
	(cd ../mvcutil; $(MAKE))

main.o: main.c++
	$(CPP) $(C_FLAGS) main.c++
ExtendedController.o: ExtendedController.c++
	$(CPP) $(C_FLAGS) ExtendedController.c++
PhongMaterial.o: PhongMaterial.c++
	$(CPP) $(C_FLAGS) PhongMaterial.c++
SceneElement.o: SceneElement.h SceneElement.c++
	$(CPP) $(C_FLAGS) SceneElement.c++
Prism.o: SceneElements/Prism.h SceneElements/Prism.c++
	$(CPP) $(C_FLAGS) SceneElements/Prism.c++
Cylinder.o: SceneElements/Cylinder.h SceneElements/Cylinder.c++
	$(CPP) $(C_FLAGS) SceneElements/Cylinder.c++
Ground.o: SceneElements/Ground.h SceneElements/Ground.c++
	$(CPP) $(C_FLAGS) SceneElements/Ground.c++
SwingSet.o: SceneElements/SwingSet.h SceneElements/SwingSet.c++
	$(CPP) $(C_FLAGS) SceneElements/SwingSet.c++
LampPost.o: SceneElements/LampPost.h SceneElements/LampPost.c++
	$(CPP) $(C_FLAGS) SceneElements/LampPost.c++
BenchArm.o: SceneElements/BenchArm.h SceneElements/BenchArm.c++
	$(CPP) $(C_FLAGS) SceneElements/BenchArm.c++
BenchChair.o: SceneElements/BenchChair.h SceneElements/BenchChair.c++
	$(CPP) $(C_FLAGS) SceneElements/BenchChair.c++
Bench.o: SceneElements/Bench.h SceneElements/Bench.c++
	$(CPP) $(C_FLAGS) SceneElements/Bench.c++
Path.o: SceneElements/Path.h SceneElements/Path.c++
	$(CPP) $(C_FLAGS) SceneElements/Path.c++
Snowman.o: SceneElements/Snowman.h SceneElements/Snowman.c++
	$(CPP) $(C_FLAGS) SceneElements/Snowman.c++
ParticleSystem.o: SceneElements/ParticleSystem.h SceneElements/ParticleSystem.c++
	$(CPP) $(C_FLAGS) SceneElements/ParticleSystem.c++