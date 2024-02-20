#include <vtkActor.h>
#include <vtkCallbackCommand.h>
#include <vtkCamera.h>
#include <vtkGlyph3D.h>
#include <vtkHardwareSelector.h>
#include <vtkMinimalStandardRandomSequence.h>
#include <vtkNew.h>
#include <vtkPointSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkSelection.h>
#include <vtkSelectionNode.h>
#include <vtkSphereSource.h>
#include <vtkTimerLog.h>

void MakeSelection(vtkSmartPointer<vtkRenderer> ren, unsigned int xmin, unsigned int ymin,
  unsigned int xmax, unsigned int ymax)
{
  if (!ren)
  {
    return;
  }
  // Pick cells
  vtkNew<vtkHardwareSelector> sel;
  sel->SetFieldAssociation(vtkDataObject::FIELD_ASSOCIATION_CELLS);
  sel->SetRenderer(ren);
  sel->SetArea(xmin, ymin, xmax, ymax);
  vtkNew<vtkTimerLog> tlog;
  tlog->StartTimer();
  vtkSelection* s = sel->Select();
  tlog->StopTimer();
  const vtkIdType numNodes = s->GetNumberOfNodes();
  for (vtkIdType nodeId = 0; nodeId < numNodes; ++nodeId)
  {
    vtkSmartPointer<vtkSelectionNode> node = s->GetNode(nodeId);
    vtkSmartPointer<vtkIdTypeArray> selIds =
      vtkArrayDownCast<vtkIdTypeArray>(node->GetSelectionList());
    std::cout << "Number of cells selected: "
              << (selIds != nullptr ? selIds->GetNumberOfTuples() : 0);
    std::cout << "\nTime for selection (sec.): " << tlog->GetElapsedTime();
  }
  s->Delete();
}

void Pick(vtkObject* caller, unsigned long eid, void* clientData, void* callData)
{
  vtkNotUsed(eid);
  vtkNotUsed(callData);

  // bool* picked = static_cast<bool*>(clientData);
  // if (*picked == true)
  //{
  //   return;
  // }
  //*picked = true;
  vtkRenderWindow* renWin = reinterpret_cast<vtkRenderWindow*>(caller);
  if (!renWin)
  {
    return;
  }
  vtkRenderer* ren = renWin->GetRenderers()->GetFirstRenderer();
  MakeSelection(ren, 0, 0, 300, 299);
}

int main(int argc, char* argv[])
{

  vtkNew<vtkMinimalStandardRandomSequence> randomSequence;
  randomSequence->SetSeed(1);

  vtkNew<vtkPointSource> pointSource;
  pointSource->SetDistributionToUniform();
  pointSource->SetNumberOfPoints(10);

  pointSource->SetOutputPointsPrecision(vtkAlgorithm::SINGLE_PRECISION);

  double center[3];
  for (unsigned int i = 0; i < 3; ++i)
  {
    randomSequence->Next();
    center[i] = randomSequence->GetValue();
  }
  pointSource->SetCenter(center);

  randomSequence->Next();
  double radius = randomSequence->GetValue();
  pointSource->SetRadius(radius);

  vtkNew<vtkSphereSource> s;
  s->SetThetaResolution(20);
  s->SetPhiResolution(20);
  vtkNew<vtkGlyph3D> glyph;
  glyph->SetInputConnection(pointSource->GetOutputPort());
  glyph->SetSourceConnection(s->GetOutputPort());
  glyph->SetScaleModeToDataScalingOff();
  glyph->SetScaleFactor(0.005);

  vtkNew<vtkRenderWindow> renWin;
  renWin->SetMultiSamples(0);
  renWin->SetSize(301, 300); // Intentional NPOT size

  vtkNew<vtkRenderer> ren;
  renWin->AddRenderer(ren);

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(glyph->GetOutputPort());
  vtkNew<vtkActor> a;
  a->SetMapper(mapper);
  ren->AddActor(a);
  ren->ResetCamera();
  ren->GetActiveCamera()->Zoom(1.8);

  renWin->Render();
  // static bool picked = false;
  // vtkNew<vtkCallbackCommand> cb;
  // cb->SetCallback(Pick);
  // cb->SetClientData(&picked);
  // renWin->AddObserver(vtkCommand::EndEvent, cb);
  //  iren->Start();

  int num_iterations = 20;
  for (int i = 1; i < num_iterations; ++i)
  {
    pointSource->SetNumberOfPoints(10 * i);
    glyph->Update();
    std::cout << "\n\nNumber of cells rendered: " << glyph->GetOutput()->GetNumberOfCells();
    renWin->Render();
    MakeSelection(ren, 0, 0, 300, 299);
  }

  return EXIT_SUCCESS;
}
