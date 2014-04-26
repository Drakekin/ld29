from cffi import FFI
import os


def load_library(ffi, library):
    extension = "dll" if os.name == "nt" else "so"
    path = os.path.join(os.path.dirname(__file__), library + "." + extension)
    return ffi.dlopen(path)


filepath = os.path.dirname(__file__)
_ffi = FFI()

# Define all the functions
_ffi.cdef("""
const char *h3dGetVersionString();
bool h3dCheckExtension( const char *extensionName );
bool h3dGetError();
bool h3dInit();
void h3dRelease();
void h3dRender( int cameraNode );
void h3dFinalizeFrame();
void h3dClear();
const char *h3dGetMessage( int *level, float *time );
float h3dGetOption( int param );
bool h3dSetOption( int param, float value );
float h3dGetStat( int param, bool reset );
void h3dShowOverlays( const float *verts, int vertCount, float colR, float colG, float colB,
                          float colA, int materialRes, int flags );
void h3dClearOverlays();
int h3dGetResType( int res );

const char *h3dGetResName( int res );
int h3dGetNextResource( int type, int start );
int h3dFindResource( int type, const char *name );
int h3dAddResource( int type, const char *name, int flags );
int h3dCloneResource( int sourceRes, const char *name );
int h3dRemoveResource( int res );
bool h3dIsResLoaded( int res );
bool h3dLoadResource( int res, const char *data, int size );
void h3dUnloadResource( int res );
int h3dGetResElemCount( int res, int elem );
int h3dFindResElem( int res, int elem, int param, const char *value );
int h3dGetResParamI( int res, int elem, int elemIdx, int param );
void h3dSetResParamI( int res, int elem, int elemIdx, int param, int value );
float h3dGetResParamF( int res, int elem, int elemIdx, int param, int compIdx );
void h3dSetResParamF( int res, int elem, int elemIdx, int param, int compIdx, float value );
const char *h3dGetResParamStr( int res, int elem, int elemIdx, int param );
void h3dSetResParamStr( int res, int elem, int elemIdx, int param, const char *value );
void *h3dMapResStream( int res, int elem, int elemIdx, int stream, bool read, bool write );
void h3dUnmapResStream( int res );
int h3dQueryUnloadedResource( int index );
void h3dReleaseUnusedResources();
int h3dCreateTexture( const char *name, int width, int height, int fmt, int flags );
void h3dSetShaderPreambles( const char *vertPreamble, const char *fragPreamble );
bool h3dSetMaterialUniform( int materialRes, const char *name, float a, float b, float c, float d );
void h3dResizePipelineBuffers( int pipeRes, int width, int height );
bool h3dGetRenderTargetData( int pipelineRes, const char *targetName, int bufIndex,
                                 int *width, int *height, int *compCount, void *dataBuffer, int bufferSize );
int h3dGetNodeType( int node );
int h3dGetNodeParent( int node );
bool h3dSetNodeParent( int node, int parent );
int h3dGetNodeChild( int node, int index );
int h3dAddNodes( int parent, int sceneGraphRes );
void h3dRemoveNode( int node );
bool h3dCheckNodeTransFlag( int node, bool reset );
void h3dGetNodeTransform( int node, float *tx, float *ty, float *tz,
                              float *rx, float *ry, float *rz, float *sx, float *sy, float *sz );
void h3dSetNodeTransform( int node, float tx, float ty, float tz,
                              float rx, float ry, float rz, float sx, float sy, float sz );
void h3dGetNodeTransMats( int node, const float **relMat, const float **absMat );
void h3dSetNodeTransMat( int node, const float *mat4x4 );
int h3dGetNodeParamI( int node, int param );
void h3dSetNodeParamI( int node, int param, int value );
float h3dGetNodeParamF( int node, int param, int compIdx );
void h3dSetNodeParamF( int node, int param, int compIdx, float value );

const char *h3dGetNodeParamStr( int node, int param );

void h3dSetNodeParamStr( int node, int param, const char *value );
int h3dGetNodeFlags( int node );
void h3dSetNodeFlags( int node, int flags, bool recursive );
void h3dGetNodeAABB( int node, float *minX, float *minY, float *minZ,
                         float *maxX, float *maxY, float *maxZ );
int h3dFindNodes( int startNode, const char *name, int type );
int h3dGetNodeFindResult( int index );
void h3dSetNodeUniforms( int node, float *uniformData, int count );
int h3dCastRay( int node, float ox, float oy, float oz, float dx, float dy, float dz, int numNearest );
bool h3dGetCastRayResult( int index, int *node, float *distance, float *intersection );
int h3dCheckNodeVisibility( int node, int cameraNode, bool checkOcclusion, bool calcLod );
int h3dAddGroupNode( int parent, const char *name );
int h3dAddModelNode( int parent, const char *name, int geometryRes );
void h3dSetupModelAnimStage( int modelNode, int stage, int animationRes, int layer,
                                 const char *startNode, bool additive );
void h3dGetModelAnimParams( int modelNode, int stage, float *time, float *weight );
void h3dSetModelAnimParams( int modelNode, int stage, float time, float weight );
bool h3dSetModelMorpher( int modelNode, const char *target, float weight );
void h3dUpdateModel( int modelNode, int flags );
int h3dAddMeshNode( int parent, const char *name, int materialRes,
                            int batchStart, int batchCount, int vertRStart, int vertREnd );
int h3dAddJointNode( int parent, const char *name, int jointIndex );
int h3dAddLightNode( int parent, const char *name, int materialRes,
                             const char *lightingContext, const char *shadowContext );
int h3dAddCameraNode( int parent, const char *name, int pipelineRes );
void h3dSetupCameraView( int cameraNode, float fov, float aspect, float nearDist, float farDist );
void h3dGetCameraProjMat( int cameraNode, float *projMat );
int h3dAddEmitterNode( int parent, const char *name, int materialRes,
                               int particleEffectRes, int maxParticleCount, int respawnCount );
void h3dUpdateEmitter( int emitterNode, float timeDelta );
bool h3dHasEmitterFinished( int emitterNode );
""")
library = load_library(_ffi, "libHorde3D")

_ffi.cdef("""
void h3dutFreeMem( char **ptr );
bool h3dutDumpMessages();
const char *h3dutGetResourcePath( int type );
void h3dutSetResourcePath( int type, const char *path );
bool h3dutLoadResourcesFromDisk( const char *contentDir );
int h3dutCreateGeometryRes( const char *name, int numVertices, int numTriangleIndices,
								   float *posData, unsigned int *indexData, short *normalData,
								   short *tangentData, short *bitangentData,
								   float *texData1, float *texData2 );
bool h3dutCreateTGAImage( const unsigned char *pixels, int width, int height, int bpp,
                              char **outData, int *outSize );
bool h3dutScreenshot( const char *filename );
void h3dutPickRay( int cameraNode, float nwx, float nwy, float *ox, float *oy, float *oz,
                       float *dx, float *dy, float *dz );
int h3dutPickNode( int cameraNode, float nwx, float nwy );
void h3dutShowText( const char *text, float x, float y, float size,
                        float colR, float colG, float colB, int fontMaterialRes );
void h3dutShowFrameStats( int fontMaterialRes, int panelMaterialRes, int mode );
""")
utilities = load_library(_ffi, "libHorde3DUtils")

