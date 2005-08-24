This is a wrapper library for UOSDK. Because UOSDK is written in .NET C#, we're unable to use its code in "simple" C(++) applications.
This wrapper dll get rid of this, it just has to be loaded dynamically into your project using GetProcAddress & friends. See uosdk.h
in support folder for its declaration.

It was written orignally for Maprenderer, to support client detection and certain methods from client class.

Use it under terms of GPL and at your own risk (see LICENSE.TXT).

If you want to modify it by your self, please take notice, that it have to be compiled with MS Visual Studio or the VSToolkit (that we used).

Have fun!

====

Developed by orphuss@crysania.de