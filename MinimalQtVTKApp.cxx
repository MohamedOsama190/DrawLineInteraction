#include <QVTKOpenGLNativeWidget.h>
#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkDoubleArray.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkPointData.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>
#include <vtkPointPicker.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRendererCollection.h>
#include <vtkLineSource.h>
#include <vtkTextActor.h>
#include <vtkTextWidget.h>
#include <vtkTextProperty.h>
#include <vtkTextRepresentation.h>



#include <QApplication>
#include <QDockWidget>
#include <QGridLayout>
#include <QLabel>
#include <QMainWindow>
#include <QPointer>
#include <QPushButton>
#include <QVBoxLayout>

#include <cmath>
#include <cstdlib>
#include <random>

std::ofstream outputFile;
std::ifstream inputFile;


vtkSmartPointer<vtkTextActor> textActor = vtkSmartPointer<vtkTextActor>::New();
vtkSmartPointer<vtkTextWidget> textWidget = vtkSmartPointer<vtkTextWidget>::New();
vtkNew<vtkLineSource> lineSource;
vtkNew<vtkActor> actor;

double a = 2.0;

namespace {

/**
 * Deform the sphere source using a random amplitude and modes and render it in
 * the window
 *
 * @param sphere the original sphere source
 * @param mapper the mapper for the scene
 * @param window the window to render to
 * @param randEng the random number generator engine
 */


void Readtxt(vtkLineSource* lineSource, vtkGenericOpenGLRenderWindow* window);
void Writetxt(vtkLineSource* lineSource, vtkGenericOpenGLRenderWindow* window);
void Color(vtkActor* actor, vtkGenericOpenGLRenderWindow* window);





class MouseInteractorStylePP : public vtkInteractorStyleTrackballCamera, QMainWindow
{
public:
    static MouseInteractorStylePP* New();
    vtkTypeMacro(MouseInteractorStylePP, vtkInteractorStyleTrackballCamera);
    int count = 0;
    double picked[3];
    double picked2[3];

    MouseInteractorStylePP()
    {
        //outputFile.open("line.txt");
        textActor->SetInput("(0, 0)");
        textActor->GetTextProperty()->SetColor(1.0, 0, 0);
        textActor->GetTextProperty()->SetFontSize(30);
        
      
       
        
        
        
    }


    virtual void OnLeftButtonDown() override
    {

        textWidget->SetInteractor(this->Interactor);
        textWidget->SetRepresentation(vtkSmartPointer<vtkTextRepresentation>::New());
        textWidget->GetTextActor()->SetPosition2(0.2, 0.15);
        textWidget->GetTextActor()->SetDisplayPosition(60, 10);
        textWidget->SelectableOff();
        this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor2D(textActor);
        textWidget->SetEnabled(true);

        std::cout << "Picking pixel: " << this->Interactor->GetEventPosition()[0]
            << " " << this->Interactor->GetEventPosition()[1] << std::endl;
        this->Interactor->GetPicker()->Pick(this->Interactor->GetEventPosition()[0],
            this->Interactor->GetEventPosition()[1],
            0, // always zero.
            this->Interactor->GetRenderWindow()
            ->GetRenderers()
            ->GetFirstRenderer());
     
       
         if (count == 0)
         {
             this->Interactor->GetPicker()->GetPickPosition(picked);
             std::cout << "Picked value: " << picked[0] << " " << picked[1] << " "
                 << picked[2] << std::endl;
             count++;
             std::string text = "(" + std::to_string(picked[0]) + "," + std::to_string(picked[1]) + ")";
             textActor->SetInput(text.c_str());
             this->Interactor->GetRenderWindow()->Render();
             return;
             
         }

         else if (count == 1)
         {
             this->Interactor->GetPicker()->GetPickPosition(picked2);
             std::cout << "Picked value: " << picked2[0] << " " << picked2[1] << " "
                 << picked2[2] << std::endl;
             count = 0;
             std::string text = "(" + std::to_string(picked2[0]) + "," + std::to_string(picked2[1]) + ")";
             textActor->SetInput(text.c_str());
         }

         // Forward events

         

        // vtkSmartPointer<vtkLineSource> lineSource = vtkSmartPointer<vtkLineSource>::New();
         lineSource->SetPoint1(picked);
         lineSource->SetPoint2(picked2);
         lineSource->Update();

         vtkNew<vtkPolyDataMapper> mapper;
         mapper->SetInputConnection(lineSource->GetOutputPort());
         //vtkNew<vtkActor> actor;
         actor->SetMapper(mapper);
         actor->GetProperty()->SetColor(0.0, 0.0, 5.0);
         this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(actor);

         //outputFile << lineSource->GetPoint1()[0] << " " << lineSource->GetPoint1()[1] << " "
        // << lineSource->GetPoint2()[0] << " " << lineSource->GetPoint2()[1] << std::endl;


         this->Interactor->GetRenderWindow()->Render();


        

        vtkInteractorStyleTrackballCamera::OnLeftButtonDown();

    }

    ~MouseInteractorStylePP()
    {
       // outputFile.close();
    }

   
};

vtkStandardNewMacro(MouseInteractorStylePP);


} // namespace

int main(int argc, char* argv[])
{
  QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());

  QApplication app(argc, argv);

  // main window
  QMainWindow mainWindow;
  mainWindow.resize(1200, 900);

  // control area
  QDockWidget controlDock;
  mainWindow.addDockWidget(Qt::LeftDockWidgetArea, &controlDock);

  QLabel controlDockTitle("Control Dock");
  controlDockTitle.setMargin(20);
  controlDock.setTitleBarWidget(&controlDockTitle);

  QPointer<QVBoxLayout> dockLayout = new QVBoxLayout();
  QWidget layoutContainer;
  layoutContainer.setLayout(dockLayout);
  controlDock.setWidget(&layoutContainer);


  QPushButton ReadtxtButton;
  ReadtxtButton.setText("Read");
  dockLayout->addWidget(&ReadtxtButton);

  QPushButton WritetxtButton;
  WritetxtButton.setText("Write");
 // WritetxtButton.setGeometry(2, 3, 400, 500);
  dockLayout->addWidget(&WritetxtButton);



  QPushButton Color;
  Color.setText("Color");
  dockLayout->addWidget(&Color);

  /*QLabel controlDockTitlee("Control Docki");
  controlDockTitlee.setMargin(20);
  controlDockTitlee.setAlignment(Qt::AlignRight);
  dockLayout->addWidget(&controlDockTitlee);*/
  

  // render area
  QPointer<QVTKOpenGLNativeWidget> vtkRenderWidget =
      new QVTKOpenGLNativeWidget();
  mainWindow.setCentralWidget(vtkRenderWidget);

  // VTK part
  vtkNew<vtkGenericOpenGLRenderWindow> window;
  vtkRenderWidget->setRenderWindow(window.Get());
  
  //vtkNew<vtkLineSource> lineSource;
  lineSource->SetPoint1(0.0, 0.0, 0.0);
  lineSource->SetPoint2(0.0, 0.0, 0.0);

  vtkNew<vtkDataSetMapper> mapper;
  mapper->SetInputConnection(lineSource->GetOutputPort());

  //vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->SetColor(0.0, 0.0, 0.5);
  

  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(actor);

  vtkNew<vtkPointPicker> pointPicker;
  window->AddRenderer(renderer);
  //window->SetInteractor(vtkRenderWidget->interactor());
  window->GetInteractor()->SetPicker(pointPicker);
 
  vtkNew<MouseInteractorStylePP> style;
  window->GetInteractor()->SetInteractorStyle(style);
 

  // connect the buttons

  QObject::connect(&ReadtxtButton, &QPushButton::released,
      [&]() { ::Readtxt(lineSource, window); });

  QObject::connect(&WritetxtButton, &QPushButton::released,
      [&]() { ::Writetxt(lineSource, window); });


  QObject::connect(&Color, &QPushButton::released,
      [&]() { ::Color(actor, window); });

  mainWindow.show();

  return app.exec();
}

namespace {




void Readtxt(vtkLineSource *lineSource, vtkGenericOpenGLRenderWindow *window)
{
    double x1, y1, x2, y2;
        inputFile.open("line.txt");
        if (inputFile.is_open()) { // always check whether the file is open
            inputFile >>x1 >>y1 >> x2 >>y2; // pipe file's content into stream
            lineSource->SetPoint1(x1, y1, 0.0);
            lineSource->SetPoint2(x2, y2, 0.0);
            window->Render();
            inputFile.close();
        }
    

}



void Writetxt(vtkLineSource* lineSource, vtkGenericOpenGLRenderWindow* window)
{
    //double x1, y1, x2, y2;
    outputFile.open("line.txt");
    if (outputFile.is_open()) { // always check whether the file is open
        outputFile << lineSource->GetPoint1()[0] << " " << lineSource->GetPoint1()[1] << " "
            << lineSource->GetPoint2()[0] << " " << lineSource->GetPoint2()[1] << std::endl;

        outputFile.close();
    }


}


void Color(vtkActor* actor, vtkGenericOpenGLRenderWindow* window)
{

    
    if (a == 2.0)
    {
        actor->GetProperty()->SetColor(1.0, 0.0, 0.0);
    }
    else if (a == 3.0)
    {
        actor->GetProperty()->SetColor(0.0, 1.0, 0.0);
    }

    else if (a == 4.0)
    {
        actor->GetProperty()->SetColor(0.0, 0.0, 1.0);
        a = 1.0;
    }

    window->Render();
        a++;


}



} // namespace
