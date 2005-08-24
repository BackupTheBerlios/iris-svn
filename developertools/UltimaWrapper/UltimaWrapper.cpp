#using <mscorlib.dll>
#using <System.dll> // implied
#using <ultima.dll>
#define DllExport  __declspec( dllexport )

using namespace Ultima;

DllExport bool ClientIsRunning()
{
	return Client::Running;
}

DllExport bool FindLocation(int *x, int *y, int *z, int *facet)
{
	Client::Calibrate();
	return Client::FindLocation(x,y,z,facet);
}

