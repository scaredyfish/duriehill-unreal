# Makefile generated by MakefileGenerator.cs
# *DO NOT EDIT*

UNREALROOTPATH = /home/andrew/bin/UE5_4
GAMEPROJECTFILE =/home/andrew/DurieHill/DurieHill.uproject

TARGETS = \
	UnrealEditor-Linux-DebugGame  \
	UnrealEditor-Linux-Shipping  \
	UnrealEditor \
	UnrealGame-Linux-DebugGame  \
	UnrealGame-Linux-Shipping  \
	UnrealGame \
	DurieHill-Linux-DebugGame  \
	DurieHill-Linux-Shipping  \
	DurieHill \
	DurieHillEditor-Linux-DebugGame  \
	DurieHillEditor-Linux-Shipping  \
	DurieHillEditor\
	configure

BUILD = bash "$(UNREALROOTPATH)/Engine/Build/BatchFiles/Linux/Build.sh"
PROJECTBUILD = "$(UNREALROOTPATH)/Engine/Binaries/ThirdParty/DotNet/6.0.302/linux/dotnet" "$(UNREALROOTPATH)/Engine/Binaries/DotNET/UnrealBuildTool/UnrealBuildTool.dll"

all: StandardSet

RequiredTools: CrashReportClient-Linux-Shipping CrashReportClientEditor-Linux-Shipping ShaderCompileWorker UnrealLightmass EpicWebHelper-Linux-Shipping

StandardSet: RequiredTools UnrealFrontend DurieHillEditor UnrealInsights

DebugSet: RequiredTools UnrealFrontend-Linux-Debug DurieHillEditor-Linux-Debug


UnrealEditor-Linux-DebugGame:
	 $(BUILD) UnrealEditor Linux DebugGame  $(ARGS)

UnrealEditor-Linux-Shipping:
	 $(BUILD) UnrealEditor Linux Shipping  $(ARGS)

UnrealEditor:
	 $(BUILD) UnrealEditor Linux Development  $(ARGS)

UnrealGame-Linux-DebugGame:
	 $(BUILD) UnrealGame Linux DebugGame  $(ARGS)

UnrealGame-Linux-Shipping:
	 $(BUILD) UnrealGame Linux Shipping  $(ARGS)

UnrealGame:
	 $(BUILD) UnrealGame Linux Development  $(ARGS)

DurieHill-Linux-DebugGame:
	 $(PROJECTBUILD) DurieHill Linux DebugGame  -project="$(GAMEPROJECTFILE)" $(ARGS)

DurieHill-Linux-Shipping:
	 $(PROJECTBUILD) DurieHill Linux Shipping  -project="$(GAMEPROJECTFILE)" $(ARGS)

DurieHill:
	 $(PROJECTBUILD) DurieHill Linux Development  -project="$(GAMEPROJECTFILE)" $(ARGS)

DurieHillEditor-Linux-DebugGame:
	 $(PROJECTBUILD) DurieHillEditor Linux DebugGame  -project="$(GAMEPROJECTFILE)" $(ARGS)

DurieHillEditor-Linux-Shipping:
	 $(PROJECTBUILD) DurieHillEditor Linux Shipping  -project="$(GAMEPROJECTFILE)" $(ARGS)

DurieHillEditor:
	 $(PROJECTBUILD) DurieHillEditor Linux Development  -project="$(GAMEPROJECTFILE)" $(ARGS)

configure:
	xbuild /property:Configuration=Development /verbosity:quiet /nologo "$(UNREALROOTPATH)/Engine/Source/Programs/UnrealBuildTool/UnrealBuildTool.csproj"
	$(PROJECTBUILD) -projectfiles -project="\"$(GAMEPROJECTFILE)\"" -game -engine 

.PHONY: $(TARGETS)
