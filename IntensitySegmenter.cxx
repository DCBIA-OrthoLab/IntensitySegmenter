/*
 * Intensity segmenter: segmentation according to intensity value
 *
 * author:  Pengdong Xiao; Beatriz Paniagua; Francois Budin
 *
 */

#include "IntensitySegmenterCLP.h"

#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkImageRegionIterator.h>
#include <itkMacro.h>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

typedef short LabelType;
typedef float InputType;

struct rangeLabel
{
  InputType range_left ;
  InputType range_right ;
  LabelType label ;
} ;

bool SaveRangeInFile( const std::vector< rangeLabel > &rangeVector ,
                      const char* defaultRangeFile
                    )
{
  std::ofstream stream_outputfile( defaultRangeFile ) ;
  if( !stream_outputfile )
  {
    return false ; // The file cannot be created
  }
  for( size_t i = 0 ; i < rangeVector.size() ; i++ )
  {
    std::string range_left_str ;
    std::string range_right_str ;
    if( rangeVector[ i ].range_left - 0.01 <= -std::numeric_limits< InputType >::max() )
    {
        range_left_str = "-infty" ;
    }
    else
    {
        std::ostringstream oss ;
        oss << rangeVector[ i ].range_left ;
        range_left_str = oss.str() ;
    }
    if( rangeVector[ i ].range_right + 0.01 >= std::numeric_limits< InputType >::max() )
    {
        range_right_str = "+infty" ;
    }
    else
    {
        std::ostringstream oss ;
        oss << rangeVector[ i ].range_right ;
        range_right_str = oss.str() ;
    }
    stream_outputfile << range_left_str << " "
                      << range_right_str << " "
                      << rangeVector[ i ].label << std::endl ;
  }
  stream_outputfile.close() ;
  return !stream_outputfile.fail() ;
}

// Parses a whole token as a number: "abc" and "12abc" are rejected
bool ParseBound( const std::string &token , InputType &value )
{
  std::istringstream iss( token ) ;
  iss >> value ;
  return !iss.fail() && ( iss >> std::ws ).eof() ;
}

const char* RangeFileErrorMessage( int status )
{
  switch( status )
  {
    case -1: return "each line must contain: left bound, right bound, label" ;
    case 1: return "the right bound must be greater than the left bound" ;
    case 2: return "ranges must be sorted and must not overlap" ;
    case 3: return "unexpected extra text at the end of a line" ;
    case 4: return "cannot open the file" ;
    case 5: return "the file contains no range" ;
    case 6: return "a range bound is not a number" ;
    default: return "unknown error" ;
  }
}

int ReadRangeFile( const char* rangeFile ,
                   std::vector< rangeLabel > &rangeVector
                 )
{
  std::ifstream stream_rangefile( rangeFile ) ;
  if( !stream_rangefile )
  {
    return 4 ; // The file cannot be opened
  }
  std::string line ;
  
  while( std::getline( stream_rangefile, line ) )
  {
    std::istringstream iss( line ) ;
    std::string range_left_str ;
    std::string range_right_str ;
    rangeLabel rl ;
    if( !(iss >> range_left_str) )
    {
      continue ; // Blank line
    }
    if( !(iss >> range_right_str >> rl.label ) )
    {
      return -1 ; // The line does not contain all the information it is suppose to
    }
    if( range_left_str == "-infty" )
    {
      rl.range_left = -std::numeric_limits< InputType >::max() ;
    }
    else if( !ParseBound( range_left_str , rl.range_left ) )
    {
      return 6 ; // The left bound is not a number
    }
    if( range_right_str == "+infty" )
    {
      rl.range_right = std::numeric_limits< InputType >::max() ;
    }
    else if( !ParseBound( range_right_str , rl.range_right ) )
    {
      return 6 ; // The right bound is not a number
    }
    if( rl.range_left >= rl.range_right )
    {
      return 1 ; // Range is incorrect: 'right' is smaller than 'left'
    }
    if( !rangeVector.empty()
       && rangeVector[ rangeVector.size() -1 ].range_right > rl.range_left )
    {
      return 2 ; // Range is incorrect: last "left" is smaller than previous "right"
    }
    std::string dummyString ;
    iss >> dummyString >> dummyString >> dummyString >> dummyString ;
    if( !dummyString.empty() )
    {
      return 3 ; // There is at least on extra string on the current line
    }
    rangeVector.push_back( rl ) ;
  }
  stream_rangefile.close() ;
  if( rangeVector.empty() )
  {
    return 5 ; // Nothing was read: every voxel would silently get the default label
  }
  return 0 ;
}

void SetDefaultLabels( std::vector< rangeLabel > &rangeVector )
{
  rangeLabel rl ;
  //Air
  rl.label = 0 ;
  rl.range_left = -std::numeric_limits< InputType >::max() ;
  rl.range_right = -700.0 ;
  rangeVector.push_back( rl ) ;
  //Lung
  rl.label = 2 ;
  rl.range_left = -700.0 ;
  rl.range_right = -300.0 ;
  rangeVector.push_back( rl ) ;
  //Soft tissue
  rl.label = 3 ;
  rl.range_left = -300.0 ;
  rl.range_right = -100.0 ;
  rangeVector.push_back( rl ) ;
  //Fat
  rl.label = 4 ;
  rl.range_left = -100.0 ;
  rl.range_right = -84.0 ;
  rangeVector.push_back( rl ) ;
  //Water
  rl.label = 5 ;
  rl.range_left = -84.0 ;
  rl.range_right = 0.0 ;
  rangeVector.push_back( rl ) ;
  //CSF
  rl.label = 6 ;
  rl.range_left = 0.0 ;
  rl.range_right = 15.0 ;
  rangeVector.push_back( rl ) ;
  //Blood and muscle
  rl.label = 7 ;
  rl.range_left = 15.0 ;
  rl.range_right = 45.0 ;
  rangeVector.push_back( rl ) ;
  //Cancellous Bone
  rl.label = 8 ;
  rl.range_left = 45.0 ;
  rl.range_right = 700.0 ;
  rangeVector.push_back( rl ) ;
  //Dense Bone
  rl.label = 9 ;
  rl.range_left = 700.0 ;
  rl.range_right = std::numeric_limits< InputType >::max() ;
  rangeVector.push_back( rl ) ;
}



int main(int argc, char *argv[])
{
  PARSE_ARGS ;
  if( defaultLabelValue < std::numeric_limits< LabelType >::min()
      || defaultLabelValue > std::numeric_limits< LabelType >::max() )
  {
    // The label image stores 16-bit labels: a larger value would wrap around silently
    std::cerr << "The default label value must be between " << std::numeric_limits< LabelType >::min()
              << " and " << std::numeric_limits< LabelType >::max() << std::endl ;
    return EXIT_FAILURE ;
  }
  std::vector< rangeLabel > rangeVector ;
  if( !defaultRangeFile.empty() )
  {
    if( !inFile.empty() || !outFile.empty() || !rangeFile.empty() )
    {
      std::cerr << "Specify either a file name to save the default range or process an image" << std::endl ;
      return EXIT_FAILURE ;
    }
    SetDefaultLabels( rangeVector ) ;
    std::cout << "Saving example range file and exiting" << std::endl ;
    if( !SaveRangeInFile( rangeVector , defaultRangeFile.c_str() ) )
    {
      std::cerr << "Cannot write the range file \"" << defaultRangeFile << "\"" << std::endl ;
      return EXIT_FAILURE ;
    }
    return EXIT_SUCCESS ;
  }
  else if( inFile.empty() || outFile.empty() )
  {
    std::cerr << "You need to specify an input and an output files" << std::endl ;
    return EXIT_FAILURE ;
  }
  if( rangeFile.empty() )
  {
    SetDefaultLabels( rangeVector ) ;
    std::cout << "Using default range values" << std::endl ;
  }
  else
  {
    // Construct intensity range
    int status = ReadRangeFile( rangeFile.c_str() , rangeVector ) ;
    if( status )
    {
      std::cerr << "Error in range file \"" << rangeFile << "\": "
                << RangeFileErrorMessage( status ) << std::endl ;
      return EXIT_FAILURE ;
    }
  }
  //Reading input image
  typedef itk::Image< InputType , 3 > InputImageType ;
  typedef itk::ImageFileReader< InputImageType > ReaderType ;
  ReaderType::Pointer reader = ReaderType::New() ;
  reader->SetFileName( inFile.c_str() ) ;
  try
  {
    reader->Update() ;
  }
  catch( itk::ExceptionObject &excp )
  {
    std::cerr << "Cannot read the input image \"" << inFile << "\": " << excp.GetDescription() << std::endl ;
    return EXIT_FAILURE ;
  }
  InputImageType::Pointer im = reader->GetOutput() ;
  //Create Label image (output)
  typedef itk::Image< LabelType , 3 > LabelImageType ;
  LabelImageType::Pointer labels = LabelImageType::New() ;
  labels->SetRegions( im->GetLargestPossibleRegion() ) ;
  labels->CopyInformation( im ) ;
  labels->Allocate() ;
  //Processing: set according to Hounsfield scale
  itk::ImageRegionIterator< InputImageType > it( im ,
					 im->GetLargestPossibleRegion() ) ;
  itk::ImageRegionIterator< LabelImageType > itLabels( labels ,
					 labels->GetLargestPossibleRegion() ) ;
  it.GoToBegin() ;
  itLabels.GoToBegin() ;
  while( !it.IsAtEnd() )
  {
    LabelType labelValue = static_cast< short > ( defaultLabelValue ) ;
    InputType pixel = it.Get() ;
    for( std::size_t i = 0 ; i < rangeVector.size() ; i++ )
    {
      if( pixel > rangeVector[ i ].range_left && pixel <= rangeVector[ i ].range_right )
      {
        labelValue = rangeVector[ i ].label ;
        break ;
      }
    }
    itLabels.Set( labelValue ) ;
    ++it ;
    ++itLabels ;
  }
  // Writing output image
  typedef itk::ImageFileWriter< LabelImageType > WriterType ;
  WriterType::Pointer writer = WriterType::New() ;
  writer->SetFileName( outFile.c_str() ) ;
  writer->SetInput( labels ) ;
  writer->UseCompressionOn() ;
  try
  {
    writer->Update() ;
  }
  catch( itk::ExceptionObject &excp )
  {
    std::cerr << "Cannot write the output image \"" << outFile << "\": " << excp.GetDescription() << std::endl ;
    return EXIT_FAILURE ;
  }

  return EXIT_SUCCESS ;
}
