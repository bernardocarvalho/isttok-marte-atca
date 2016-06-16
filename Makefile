OBJS = linux/AdvancedConfigurator.o linux/SVGGraphicSupport.o linux/IWaveform.o linux/IntegerSequentialControl.o linux/IPID.o linux/UFSerialUART.o linux/MainConfigurator.o linux/WaveformGAM.o linux/TimeWindowsGAM.o linux/TomographyGAM.o linux/ElectricProbesGAM.o linux/MagneticsGAM.o linux/SineProbeGAM.o linux/CosineProbeGAM.o linux/MainRogowskiGAM.o linux/InterferometryGAM.o linux/HAlphaGAM.o linux/TechnicalSignalsGAM.o linux/MachineProtectionGAM.o linux/PlasmaStatusGAM.o linux/ControllerGAM.o linux/PowerSupplyCommunicatorGAM.o linux/FireSignalDischargeStatusGAM.o linux/HeavyIonBeamGAM.o linux/ElectrodeBiasingGAM.o

#CFLAGS = -m32
CFLAGS = -fPIC
CFLAGS += -I.
CFLAGS += -I$(JETCONTROLPATH)/BaseLib2/Level0
CFLAGS += -I$(JETCONTROLPATH)/BaseLib2/Level1
CFLAGS += -I$(JETCONTROLPATH)/BaseLib2/Level2
CFLAGS += -I$(JETCONTROLPATH)/BaseLib2/Level3
CFLAGS += -I$(JETCONTROLPATH)/BaseLib2/Level4
CFLAGS += -I$(JETCONTROLPATH)/BaseLib2/Level5
CFLAGS += -I$(JETCONTROLPATH)/BaseLib2/Level6



LIBRARY_PATH += -L$(JETCONTROLPATH)/BaseLib2/linux
LIBRARIES = -lBaseLib2


CFLAGSPEC= -D_LINUX -DUSE_PTHREAD -pthread


all : $(OBJS)
	gcc -O3 $(CFLAGS) $(LIBRARY_PATH) -shared -fPIC $(OBJS) $(LIBRARIES) -o linux/isttokbiblio.so
	touch linux/libisttokbiblio.so
	rm linux/libisttokbiblio.so
	ln -fns isttokbiblio.so linux/libisttokbiblio.so


linux/%.o : %.cpp
	gcc -O3 -c $(CFLAGS) $(CFLAGSPEC) $(LIBRARY_PATH) $(LIBRARIES) $*.cpp -o linux/$*.o


clean:
	rm -f linux/depends*
	rm -f linux/*.o
	rm -f linux/*.a
	rm -f linux/*.so
	rm -f linux/*.ex
