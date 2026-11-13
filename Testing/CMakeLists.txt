include(ExternalData)

set(CLP IntensitySegmenter )

add_executable(${CLP}Test ${CLP}Test.cxx)
target_link_libraries(${CLP}Test ${CLP}Lib )
set_target_properties(${CLP}Test PROPERTIES LABELS ${CLP})


# Add standard remote object stores to user's
# configuration.
list(APPEND ExternalData_URL_TEMPLATES
  "https://github.com/Slicer/SlicerTestingData/releases/download/MD5/%(hash)"
  )

Add_Test(  NAME ${CLP}TestHelp
           COMMAND $<TARGET_FILE:${CLP}> --help
        )

Add_Test(  NAME ${CLP}TestSaveDefaultRange
           COMMAND $<TARGET_FILE:${CLP}> -s ${CMAKE_CURRENT_BINARY_DIR}/defaultRange.txt
        )

set( output${CLP}TestRunDefaultOutputVolume ${CMAKE_CURRENT_BINARY_DIR}/SegmentedPreDentalSurgery.nii.gz )
ExternalData_Add_Test( ${CLP}Data
  NAME ${CLP}TestRunDefaultParameters
  COMMAND $<TARGET_FILE:${CLP}Test> --compare DATA{Data/Segmentation_presurg_IntensitySegmenterDefaultRange.nii.gz}
  ${output${CLP}TestRunDefaultOutputVolume}
  --compareIntensityTolerance 0
  --compareRadiusTolerance 0
  --compareNumberOfPixelsTolerance 0
  ModuleEntryPoint
  -i DATA{Data/Greyscale_presurg.gipl.gz}
  -o ${output${CLP}TestRunDefaultOutputVolume}
  )

set( ${CLP}TestFloatIntensityRangeOutputVolume ${CMAKE_CURRENT_BINARY_DIR}/TestFloatIntensityRangeOutputVolume.nrrd )
ExternalData_Add_Test( ${CLP}Data
  NAME ${CLP}TestFloatIntensityRange
  COMMAND $<TARGET_FILE:${CLP}Test> --compare DATA{Data/TestFloatIntensityRangeOutputVolume.nrrd}
  ${${CLP}TestFloatIntensityRangeOutputVolume}
  --compareIntensityTolerance 0
  --compareRadiusTolerance 0
  --compareNumberOfPixelsTolerance 0
  ModuleEntryPoint
  -i DATA{Data/TestFloatIntensityRangeInputVolume.nrrd}
  -o ${${CLP}TestFloatIntensityRangeOutputVolume}
  -r ${CMAKE_CURRENT_SOURCE_DIR}/Data/TestFloatIntensityRange.txt
  )


set( ${CLP}DefaultLabelValueOutputVolume ${CMAKE_CURRENT_BINARY_DIR}/TestDefaultLabelValueOutputVolume.nrrd )
ExternalData_Add_Test( ${CLP}Data
  NAME ${CLP}TestDefaultLabelValue
  COMMAND $<TARGET_FILE:${CLP}Test> --compare DATA{Data/TestDefaultLabelValueOutputVolume.nrrd}
  ${${CLP}DefaultLabelValueOutputVolume}
  --compareIntensityTolerance 0
  --compareRadiusTolerance 0
  --compareNumberOfPixelsTolerance 0
  ModuleEntryPoint
  -i DATA{Data/Greyscale_presurg.gipl.gz}
  -o ${${CLP}DefaultLabelValueOutputVolume}
  -r ${CMAKE_CURRENT_SOURCE_DIR}/Data/RangeWithHoles-defaultLabelTest.txt
  -d 1
  )

############ Tests failing on purpose #######################
set( ${CLP}TestSaveDefaultRangeSpecifyInputAndOutput ${CMAKE_CURRENT_BINARY_DIR}/TestSaveDefaultRangeSpecifyInputAndOutput.nrrd )
ExternalData_Add_Test( ${CLP}Data
  NAME ${CLP}TestSaveDefaultRangeSpecifyInputAndOutput
  COMMAND $<TARGET_FILE:${CLP}> -s ${CMAKE_CURRENT_BINARY_DIR}/defaultRange.txt -i DATA{Data/Greyscale_presurg.gipl.gz} -o ${${CLP}TestSaveDefaultRangeSpecifyInputAndOutput}
  )
set_tests_properties(${CLP}TestSaveDefaultRangeSpecifyInputAndOutput PROPERTIES WILL_FAIL true)

set( ${CLP}TestFailingNoInput ${CMAKE_CURRENT_BINARY_DIR}/TestFailingNoInput.nrrd )
Add_Test( NAME ${CLP}TestNoInput
          COMMAND $<TARGET_FILE:${CLP}> -o ${${CLP}TestFailingNoInput}
        )
set_tests_properties(${CLP}TestNoInput PROPERTIES WILL_FAIL true)

ExternalData_Add_Test( ${CLP}Data
  NAME ${CLP}TestNoOutput
  COMMAND $<TARGET_FILE:${CLP}> -i DATA{Data/Greyscale_presurg.gipl.gz}
  )
set_tests_properties(${CLP}TestNoOutput PROPERTIES WILL_FAIL true)
#############################################################


############ Errors must be reported, never silently ignored #############
# Each of these used to end with "success" (exit code 0, default label on every voxel) or with an abort.
Add_Test( NAME ${CLP}TestRangeFileNotFound
          COMMAND $<TARGET_FILE:${CLP}> -i ${CMAKE_CURRENT_BINARY_DIR}/none.nrrd -o ${CMAKE_CURRENT_BINARY_DIR}/none-out.nrrd
                  -r ${CMAKE_CURRENT_BINARY_DIR}/does-not-exist.txt
        )
set_tests_properties(${CLP}TestRangeFileNotFound PROPERTIES PASS_REGULAR_EXPRESSION "cannot open the file")

Add_Test( NAME ${CLP}TestEmptyRangeFile
          COMMAND $<TARGET_FILE:${CLP}> -i ${CMAKE_CURRENT_BINARY_DIR}/none.nrrd -o ${CMAKE_CURRENT_BINARY_DIR}/none-out.nrrd
                  -r ${CMAKE_CURRENT_SOURCE_DIR}/Data/EmptyRange.txt
        )
set_tests_properties(${CLP}TestEmptyRangeFile PROPERTIES PASS_REGULAR_EXPRESSION "contains no range")

Add_Test( NAME ${CLP}TestNonNumericRange
          COMMAND $<TARGET_FILE:${CLP}> -i ${CMAKE_CURRENT_BINARY_DIR}/none.nrrd -o ${CMAKE_CURRENT_BINARY_DIR}/none-out.nrrd
                  -r ${CMAKE_CURRENT_SOURCE_DIR}/Data/NonNumericRange.txt
        )
set_tests_properties(${CLP}TestNonNumericRange PROPERTIES PASS_REGULAR_EXPRESSION "is not a number")

Add_Test( NAME ${CLP}TestInputImageNotFound
          COMMAND $<TARGET_FILE:${CLP}> -i ${CMAKE_CURRENT_BINARY_DIR}/does-not-exist.nrrd -o ${CMAKE_CURRENT_BINARY_DIR}/none-out.nrrd
        )
set_tests_properties(${CLP}TestInputImageNotFound PROPERTIES PASS_REGULAR_EXPRESSION "Cannot read the input image")

Add_Test( NAME ${CLP}TestDefaultRangeFileNotWritable
          COMMAND $<TARGET_FILE:${CLP}> -s ${CMAKE_CURRENT_BINARY_DIR}/no-such-directory/range.txt
        )
set_tests_properties(${CLP}TestDefaultRangeFileNotWritable PROPERTIES PASS_REGULAR_EXPRESSION "Cannot write the range file")

Add_Test( NAME ${CLP}TestDefaultLabelValueOutOfRange
          COMMAND $<TARGET_FILE:${CLP}> -i ${CMAKE_CURRENT_BINARY_DIR}/none.nrrd -o ${CMAKE_CURRENT_BINARY_DIR}/none-out.nrrd -d 40000
        )
set_tests_properties(${CLP}TestDefaultLabelValueOutOfRange PROPERTIES PASS_REGULAR_EXPRESSION "default label value must be between")

# Blank lines in a range file are ignored (the run must succeed)
set( ${CLP}BlankLinesOutputVolume ${CMAKE_CURRENT_BINARY_DIR}/TestBlankLinesOutputVolume.nrrd )
ExternalData_Add_Test( ${CLP}Data
  NAME ${CLP}TestRangeFileWithBlankLines
  COMMAND $<TARGET_FILE:${CLP}> -i DATA{Data/Greyscale_presurg.gipl.gz}
  -o ${${CLP}BlankLinesOutputVolume}
  -r ${CMAKE_CURRENT_SOURCE_DIR}/Data/RangeWithBlankLines.txt
  )
#############################################################


# Add a build target to populate the real data.
ExternalData_Add_Target( ${CLP}Data )
