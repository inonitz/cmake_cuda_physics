@echo off
REM TODO REQUIRES TESTING ON WINDOWS


SET "PROJECT_NAME=testing_cuda_with_cmake"
SET "CMAKE_ARGLIST= ^
    -DCMAKE_C_COMPILER=clang-20 ^
    -DCMAKE_CXX_COMPILER=clang++-20 ^
    -DCMAKE_EXPORT_COMPILE_COMMANDS=1 ^
    -DGIT_SUBMODULE=1 ^
    -DCMAKE_COLOR_DIAGNOSTICS=ON ^
    -DSDLSHADERCROSS_DXC=OFF ^
    -DSDL3_DIR=dependencies/SDL3 ^
"

SET "CMAKE_ORIGINAL_SCRIPT_PATH="%cd%""
SET "CMAKE_ROOT_BUILD_DIR=build"
SET "CMAKE_INTRMD_BUILD_DIR="
SET "CLEAN_CURRENT_ROOT_BUILD_DIR=true"
SET "RUN_BINARY_FLAG=false"
SET "BUILD_BINARIES_FLAG=false"




IF "%#%" NEQ "3" (
  echo "3s Arguments required to run the script"
  exit
)


REM argument parsing
IF "%~1" "=" "debug" (
  CMAKE_ARGLIST+= -DCMAKE_BUILD_TYPE=Debug
  CMAKE_INTRMD_BUILD_DIR+=debug/
  
) ELSE (
  IF "%~1" "=" "debug_perf" (
    CMAKE_ARGLIST+= -DCMAKE_BUILD_TYPE=Debug
    CMAKE_ARGLIST+= -DMEASURE_PERFORMANCE_TIMEOUT=1
    CMAKE_INTRMD_BUILD_DIR+=debug_perf/
    
  ) ELSE (
    IF "%~1" "=" "release" (
      CMAKE_ARGLIST+= -DCMAKE_BUILD_TYPE=Release
      CMAKE_INTRMD_BUILD_DIR+=release/
      
    ) ELSE (
      IF "%~1" "=" "release_dbginfo" (
        CMAKE_ARGLIST+= -DCMAKE_BUILD_TYPE=RelWithDbgInfo
        CMAKE_INTRMD_BUILD_DIR+=release_dbginfo/
        
      ) ELSE (
        IF "%~1" "=" "release_perf" (
          CMAKE_ARGLIST+= -DCMAKE_BUILD_TYPE=Release
          CMAKE_ARGLIST+= -DMEASURE_PERFORMANCE_TIMEOUT=1
          CMAKE_INTRMD_BUILD_DIR+=release_perf/
          
        ) ELSE (
          printf "Unknown Argument %s - valid values are: debug, release, release_dbginfo
          Exiting..." "%~1"
          exit
        )
      )
    )
  )
)


IF "%~2" "=" "shared" (
  CMAKE_ARGLIST+= -DBUILD_SHARED_LIBS=1
  CMAKE_INTRMD_BUILD_DIR+=shared\
  
) ELSE (
  IF "%~2" "=" "static" (
    CMAKE_ARGLIST+= -DBUILD_SHARED_LIBS=0
    CMAKE_INTRMD_BUILD_DIR+=static\
    
  ) ELSE (
    printf "Unknown Argument %s - valid values are: shared, static
    Exiting..." "%~2"
    exit
  )
)


IF "%~3" "=" "run" (
  SET "CLEAN_CURRENT_ROOT_BUILD_DIR=false"
  SET "BUILD_BINARIES_FLAG=false"
  SET "RUN_BINARY_FLAG=true"
) ELSE (
  IF "%~3" "=" "cleanbuild" (
    SET "CLEAN_CURRENT_ROOT_BUILD_DIR=true"
    SET "BUILD_BINARIES_FLAG=true"
    SET "RUN_BINARY_FLAG=false"
  ) ELSE (
    IF "%~3" "=" "build" (
      SET "CLEAN_CURRENT_ROOT_BUILD_DIR=false"
      SET "BUILD_BINARIES_FLAG=true"
      SET "RUN_BINARY_FLAG=false"
    ) ELSE (
      echo "Unknown Argument '%~3' - valid values are: run, cleanbuild, build"
      echo:
      echo "Exiting..."
      exit
    )
  )
)




REM the actual script
SET "CMAKE_FINAL_BUILD_DIR=%CMAKE_ROOT_BUILD_DIR%/%CMAKE_INTRMD_BUILD_DIR%"
echo "Out-of-source Root   Build Directory is '%CMAKE_ROOT_BUILD_DIR%' "
echo "Out-of-source Target Build Directory is '%CMAKE_FINAL_BUILD_DIR%' "
echo "Cmake Arguments passed are ==> { %CMAKE_ARGLIST% }"
echo "Script arguments are '%~1' '%~2' '%~3' "


mkdir "build"
IF "%CLEAN_CURRENT_ROOT_BUILD_DIR%" "=" "true" (
  DEL /S "%CMAKE_FINAL_BUILD_DIR%"
)


IF "%BUILD_BINARIES_FLAG%" "=" "true" (
  mkdir "%CMAKE_FINAL_BUILD_DIR%"
  cmake -S "." -B "%CMAKE_FINAL_BUILD_DIR%" -G "Ninja" "%CMAKE_ARGLIST%"
  
  cd "%CMAKE_FINAL_BUILD_DIR%"
  COPY  "compile_commands.json" "%CD%\..\..\compile_commands.json"
  ninja "%PROJECT_NAME%"
)


IF "%RUN_BINARY_FLAG%" "=" "true" (
  cd "%CMAKE_FINAL_BUILD_DIR%"
  ninja "run_binary"
)
