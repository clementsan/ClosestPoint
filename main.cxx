#include <iostream>
#include <vector>

#include <vtkPolyDataReader.h>
#include <vtkPolyDataWriter.h>
#include <vtkPolyData.h>
#include <vtkCell.h>
#include <vtkPoints.h>

int main(int argc, char *argv[])
{

  if (argc != 5)
    {
      std::cerr<<argv[0]<<" <InputMesh1> <InputAttributeFile1> <InputMesh2> <OutputAttributeFile>"<<std::endl;
      return EXIT_FAILURE;
    }
  else
    {
      // Reading input mesh 1
      std::cout<<"Reading input mesh 1..."<<std::endl;
      vtkPolyDataReader *meshReader1 = vtkPolyDataReader::New();
      meshReader1->SetFileName(argv[1]);
      meshReader1->Update();
      vtkPolyData *mesh1 = meshReader1->GetOutput();

      // Reading input mesh 2
      std::cout<<"Reading input mesh 2..."<<std::endl;
      vtkPolyDataReader *meshReader2 = vtkPolyDataReader::New();
      meshReader2->SetFileName(argv[3]);
      meshReader2->Update();
      vtkPolyData *mesh2 = meshReader2->GetOutput();
      
      //Reading attribute file
      std::cout<<"Reading attribute file..."<<std::endl;
      std::ifstream infile;
      char Line[40];
      float CurrentAttribute;
      std::vector<float> v_AttributeIn, v_AttributeOut;
      infile.open (argv[2]) ;
      if (!infile)
	{
	  std::cerr << "Unable to open file: "<<argv[2]<<std::endl;
	  exit(1);
	}
      while ( std::strncmp (Line, "NUMBER_OF_POINTS =", 18) && strncmp (Line, "NUMBER_OF_POINTS=", 17))
	infile.getline (Line, 40);
      unsigned int NbVertices = atoi(strrchr(Line,'=')+1);
      infile.getline(Line, 40);
      infile.getline(Line, 40);      
      for (unsigned int i = 0; i < NbVertices; i++)
	{
	  infile >> CurrentAttribute;
	  v_AttributeIn.push_back(CurrentAttribute);
	}
      infile.close();
      
      // Computing closest points
      std::cout<<"Computing closest points..."<<std::endl;
      double x[3];
      vtkIdType closestVertId;
      for (int PointId = 0; PointId < mesh2->GetNumberOfPoints(); PointId++)
	{	  
	  mesh2->GetPoint(PointId,x);
	  closestVertId = mesh1->FindPoint(x);
	  if (closestVertId >= 0)
	    v_AttributeOut.push_back(v_AttributeIn[(int)closestVertId]);
	  else 
	    {
	      std::cerr << "Error: no closest point for vertex" << PointId << std::endl ;
	      return EXIT_FAILURE;
	    }
	}
      
      // Writing attribute file
      std::cout<<"Writing attribute file..."<<std::endl;
      std::ofstream outfile;
      outfile.open ( argv[4]) ;
      outfile << "NUMBER_OF_POINTS=" << v_AttributeOut.size() << std::endl ;
      outfile << "DIMENSION=" << 1 << std::endl ;
      outfile << "TYPE=Scalar" << std::endl ;
      for (unsigned int i = 0; i < v_AttributeOut.size(); i++)
	outfile  << v_AttributeOut[i] << std::endl;
      outfile.close();      
    }
  return EXIT_SUCCESS;
}
