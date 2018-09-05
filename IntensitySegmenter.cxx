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
#include <limits>

typedef short LabelType;
typedef float InputType;

struct rangeLabel
{
  InputType range_left ;
  InputType range_right ;
  LabelType label ;
} ;

void SaveRangeInFile( std::vector< rangeLabel > rangeVector ,
                      const char* defaultRangeFile
                    )
{
  std::ofstream stream_outputfile( defaultRangeFile ) ;
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
}

int ReadRangeFile( const char* rangeFile ,
                   std::vector< rangeLabel > &rangeVector
                 )
{
  std::ifstream stream_rangefile( rangeFile ) ;
  std::string line ;
  
  while( std::getline( stream_rangefile, line ) )
  {
    std::istringstream iss( line ) ;
    std::string range_left_str ;
    std::string range_right_str ;
    rangeLabel rl ;
    if( !(iss >> range_left_str >> range_right_str >> rl.label ) )
    {
      return -1 ; // The line does not contain all the information it is suppose to
    }
    if( range_left_str == "-infty" )
    {
      rl.range_left = -std::numeric_limits< InputType >::max() ;
    }
    else
    {
      std::istringstream iss_left( range_left_str ) ;
      iss_left >> rl.range_left ;
    }
    if( range_right_str == "+infty" )
    {
      rl.range_right = std::numeric_limits< InputType >::max() ;
    }
    else
    {
      std::istringstream iss_right( range_right_str ) ;
      iss_right >> rl.range_right ;
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
    SaveRangeInFile( rangeVector , defaultRangeFile.c_str() ) ;
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
    if( ReadRangeFile( rangeFile.c_str() , rangeVector ) )
    {
      std::cerr << "Error in range file" << std::endl ;
      return EXIT_FAILURE ;
    }
  }
  //Reading input image
  typedef itk::Image< InputType , 3 > InputImageType ;
  typedef itk::ImageFileReader< InputImageType > ReaderType ;
  ReaderType::Pointer reader = ReaderType::New() ;
  reader->SetFileName( inFile.c_str() ) ;
  reader->Update() ;
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
  writer->Update() ;

  return EXIT_SUCCESS ;
}
