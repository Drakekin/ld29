class Options(object):
    DumpFailedShaders = 13
    WireframeMode = 11
    MaxAnisotropy = 4
    SampleCount = 10
    FastAnimation = 8
    GatherTimeStats = 14
    LoadTextures = 7
    SRGBLinearization = 6
    MaxLogLevel = 1
    MaxNumMessages = 2
    TrilinearFiltering = 3
    ShadowMapSize = 9
    TexCompression = 5
    DebugViewMode = 12


class Stats(object):
    AnimationTime = 104
    GeoUpdateTime = 105
    FwdLightsGPUTime = 107
    ParticleSimTime = 106
    TriCount = 100
    LightPassCount = 102
    DefLightsGPUTime = 108
    ParticleGPUTime = 110
    FrameTime = 103
    ShadowsGPUTime = 109
    BatchCount = 101
    GeometryVMem = 112
    TextureVMem = 111


class ResTypes(object):
    Pipeline = 9
    Code = 5
    Undefined = 0
    Geometry = 2
    Material = 4
    Shader = 6
    Texture = 7
    Animation = 3
    ParticleEffect = 8
    SceneGraph = 1


class ResFlags(object):
    NoQuery = 1
    TexCubemap = 8
    TexRenderable = 32
    TexSRGB = 64
    TexDynamic = 16
    NoTexCompression = 2
    NoTexMipmaps = 4


class Formats(object):
    TEX_RGBA32F = 6
    Unknown = 0
    TEX_RGBA16F = 5
    TEX_DXT5 = 4
    TEX_DXT3 = 3
    TEX_DXT1 = 2
    TEX_BGRA8 = 1


class GeoRes(object):
    GeoVertPosStream = 205
    GeometryElem = 200
    GeoVertexCountI = 202
    GeoIndices16I = 203
    GeoIndexCountI = 201
    GeoIndexStream = 204
    GeoVertStaticStream = 207
    GeoVertTanStream = 206


class AnimRes(object):
    EntityElem = 300
    EntFrameCountI = 301


class MatRes(object):
    MatSampTexResI = 407
    MatSampNameStr = 406
    MatMatShaderI = 405
    MatUnifNameStr = 408
    MatMatLinkI = 404
    MatMatClassStr = 403
    MatUnifValueF4 = 409
    MaterialElem = 400
    MatUniformElem = 402
    MatSamplerElem = 401


class ShaderRes(object):
    SampNameStr = 604
    UnifSizeI = 607
    ContextElem = 600
    UnifDefValueF4 = 608
    SampDefTexResI = 605
    SamplerElem = 601
    ContNameStr = 603
    UniformElem = 602
    UnifNameStr = 606


class TexRes(object):
    ImgWidthI = 704
    TexSliceCountI = 703
    ImageElem = 701
    ImgHeightI = 705
    TextureElem = 700
    ImgPixelStream = 706
    TexFormatI = 702


class PartEffRes(object):
    ChanStartMinF = 810
    ChanColBElem = 806
    ChanEndRateF = 812
    ChanStartMaxF = 811
    ChanColAElem = 807
    ChanMoveVelElem = 801
    PartLifeMinF = 808
    ParticleElem = 800
    ChanColRElem = 804
    ChanSizeElem = 803
    ChanColGElem = 805
    ChanDragElem = 813
    PartLifeMaxF = 809
    ChanRotVelElem = 802


class PipeRes(object):
    StageElem = 900
    StageNameStr = 901
    StageActivationI = 902


class NodeTypes(object):
    Camera = 6
    Group = 1
    Undefined = 0
    Light = 5
    Joint = 4
    Mesh = 3
    Model = 2
    Emitter = 7


class NodeFlags(object):
    NoCastShadow = 2
    NoDraw = 1
    Inactive = 7
    NoRayQuery = 4


class NodeParams(object):
    NameStr = 1
    AttachmentStr = 2


class Model(object):
    LodDist1F = 202
    LodDist2F = 203
    LodDist3F = 204
    AnimCountI = 206
    LodDist4F = 205
    GeoResI = 200
    SWSkinningI = 201


class Mesh(object):
    VertREndI = 304
    BatchStartI = 301
    VertRStartI = 303
    BatchCountI = 302
    MatResI = 300
    LodLevelI = 305


class Joint(object):
    JointIndexI = 400


class Light(object):
    FovF = 502
    ShadowMapCountI = 505
    LightingContextStr = 508
    ColorMultiplierF = 504
    RadiusF = 501
    ShadowMapBiasF = 507
    ColorF3 = 503
    MatResI = 500
    ShadowSplitLambdaF = 506
    ShadowContextStr = 509


class Camera(object):
    BottomPlaneF = 605
    OrthoI = 613
    NearPlaneF = 607
    ViewportWidthI = 611
    ViewportXI = 609
    FarPlaneF = 608
    LeftPlaneF = 603
    ViewportYI = 610
    OccCullingI = 614
    ViewportHeightI = 612
    RightPlaneF = 604
    OutTexResI = 601
    PipeResI = 600
    TopPlaneF = 606
    OutBufIndexI = 602


class Emitter(object):
    ForceF3 = 707
    PartEffResI = 701
    MatResI = 700
    RespawnCountI = 703
    SpreadAngleF = 706
    MaxCountI = 702
    EmissionRateF = 705
    DelayF = 704


class ModelUpdateFlags(object):
    Geometry = 2
    Animation = 1