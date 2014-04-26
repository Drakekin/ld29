from .ffi import library, _ffi
from . import ut
from .enums import (
    Options, Stats, ResTypes, ResFlags, Formats, GeoRes,
    AnimRes, MatRes, ShaderRes, TexRes, PartEffRes,
    PipeRes, NodeTypes, NodeFlags, NodeParams, Model,
    Mesh, Joint, Light, Camera, Emitter, ModelUpdateFlags
)

checkExtension = library.h3dCheckExtension
getError = library.h3dGetError
init = library.h3dInit
release = library.h3dRelease
render = library.h3dRender
finalizeFrame = library.h3dFinalizeFrame
clear = library.h3dClear


def getMessage():
    """
    :return: tuple of the message string, the importance level and the time it was stored.
             Will return ("", 0, 0.0) if there is no message.
    """
    level = _ffi.new("int *")
    timestamp = _ffi.new("float *")
    message = library.h3dGetMessage(level, timestamp)
    return _ffi.string(message), level[0], timestamp[0]


getOption = library.h3dGetOption
setOption = library.h3dSetOption
getStat = library.h3dGetStat


def showOverlays(vertices, color, material):
    """
    :param vertices: A list of four tuple of normalised floats between 0 and 1 in format (x, y, u, v)
    :param color: A four tuple of floats representing a colour in format RGBA
    :param material: A material resource
    """
    verts = len(vertices)
    if not verts % 4:
        raise ValueError("vertices must be in groups of four")
    vertList = list(sum(vertices, ()))
    vertArray = _ffi.new("float[]", vertList)
    r, g, b, a = color
    library.h3dShowOverlays(vertArray, verts, r, g, b, a, material, 0)


clearOverlays = library.h3dClearOverlays
getResType = library.h3dGetResType


def getResName(resource):
    return _ffi.string(library.h3dGetResName(resource))


getNextResource = library.h3dGetNextResource
findResource = library.h3dFindResource
addResource = library.h3dAddResource
cloneResource = library.h3dCloneResource
removeResource = library.h3dRemoveResource
isResLoaded = library.h3dIsResLoaded
loadResource = library.h3dLoadResource
unloadResource = library.h3dUnloadResource
getResElemCount = library.h3dGetResElemCount
findResElem = library.h3dFindResElem
getResParamI = library.h3dGetResParamI
setResParamI = library.h3dSetResParamI
getResParamF = library.h3dGetResParamF
setResParamF = library.h3dSetResParamF
getResParamStr = library.h3dGetResParamStr
setResParamStr = library.h3dSetResParamStr
mapResStream = library.h3dMapResStream
unmapResStream = library.h3dUnmapResStream
queryUnloadedResource = library.h3dQueryUnloadedResource
releaseUnusedResources = library.h3dReleaseUnusedResources
createTexture = library.h3dCreateTexture
setShaderPreambles = library.h3dSetShaderPreambles
setMaterialUniform = library.h3dSetMaterialUniform
resizePipelineBuffers = library.h3dResizePipelineBuffers
getRenderTargetData = library.h3dGetRenderTargetData
getNodeType = library.h3dGetNodeType
getNodeParent = library.h3dGetNodeParent
setNodeParent = library.h3dSetNodeParent
getNodeChild = library.h3dGetNodeChild
addNodes = library.h3dAddNodes
removeNode = library.h3dRemoveNode
checkNodeTransFlag = library.h3dCheckNodeTransFlag
getNodeTransform = library.h3dGetNodeTransform
setNodeTransform = library.h3dSetNodeTransform
getNodeTransMats = library.h3dGetNodeTransMats
setNodeTransMat = library.h3dSetNodeTransMat
getNodeParamI = library.h3dGetNodeParamI
setNodeParamI = library.h3dSetNodeParamI
getNodeParamF = library.h3dGetNodeParamF
setNodeParamF = library.h3dSetNodeParamF
setNodeParamStr = library.h3dSetNodeParamStr
getNodeFlags = library.h3dGetNodeFlags
setNodeFlags = library.h3dSetNodeFlags
getNodeAABB = library.h3dGetNodeAABB
findNodes = library.h3dFindNodes
getNodeFindResult = library.h3dGetNodeFindResult
setNodeUniforms = library.h3dSetNodeUniforms
castRay = library.h3dCastRay
getCastRayResult = library.h3dGetCastRayResult
checkNodeVisibility = library.h3dCheckNodeVisibility
addGroupNode = library.h3dAddGroupNode
addModelNode = library.h3dAddModelNode
setupModelAnimStage = library.h3dSetupModelAnimStage
getModelAnimParams = library.h3dGetModelAnimParams
setModelAnimParams = library.h3dSetModelAnimParams
setModelMorpher = library.h3dSetModelMorpher
updateModel = library.h3dUpdateModel
addMeshNode = library.h3dAddMeshNode
addJointNode = library.h3dAddJointNode
addLightNode = library.h3dAddLightNode
addCameraNode = library.h3dAddCameraNode
setupCameraView = library.h3dSetupCameraView
getCameraProjMat = library.h3dGetCameraProjMat
addEmitterNode = library.h3dAddEmitterNode
updateEmitter = library.h3dUpdateEmitter
hasEmitterFinished = library.h3dHasEmitterFinished

