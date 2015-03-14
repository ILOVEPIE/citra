
// Copyright 2014 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once

#include <common/common_types.h>

#include "pica.h"

namespace Pica{

namespace SSR{
    
namespace SPIRV{
#if defined(_WIN32)
#define SPIR_LE
#elseif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define SPIR_LE
#elseif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define SPIR_BE
#elseif __BYTE_ORDER__ == __ORDER_PDP_ENDIAN__
#define SPIR_MLE
#else
#define SPIR_MBE
#endif
    
#if defined(SPIRV_LE)

const u8 * _le_magic = {u8(0x03),u8(0x02),u8(0x23),u8(0x07)};
#define MAGIC *((u32*)_le_magic)
#define MAGIC_EXTERN *((u32*)SPIRV::_le_magic)

#elseif defined(SPIRV_MBE)

const u8 * _mbe_magic = {u8(0x23),u8(0x07),u8(0x03),u8(0x02)};
#define MAGIC *((u32*)_mbe_magic)
#define MAGIC_EXTERN *((u32*)SPIRV::_mbe_magic)

#elseif defined(SPIRV_MLE)

const u8 * _mle_magic = {u8(0x02),u8(0x03),u8(0x07),u8(0x23)};
#define MAGIC *((u32*)_mle_magic)
#define MAGIC_EXTERN *((u32*)SPIRV::_mle_magic)

#else //#ifdef SPIRV_BE

const u8 * _be_magic = {u8(0x07),u8(0x23),u8(0x02),u8(0x03)};
#define MAGIC *((u32*)_be_magic)
#define MAGIC_EXTERN *((u32*)SPIRV::_be_magic)

#endif


struct Module{
    u32 magic;
    u32 version;
    u32 compiler_magic;
    u32 upper_bound;
    u32 reserved_inst_schema;
};
    
typedef union{
    u32 word_representation;
#if defined(SPIRV_BE)||defined(SIPR_MBE)
    struct{ u16 word_count, u16 opcode } instruction;
#else //#if defined(SPIRV_LE)||defined(SPIRV_MLE)
    struct{ u16 opcode, u16 word_count } instruction;
#endif
}* Instruction;


enum class InstructionCapabilities{
    ALL = 0,
    Matrix = 1,//1
    Shader = 3,//2
    Geom = 7,//4
    Tess = 11,//8
    Addr = 16,//16
    Link = 32,//32
    Kernel = 64,//64
}

class InstructionType{
public:
    InstructionType(u16 word_count,u16 opcode,bool hasid, bool hasresultid, short capability_flags = InstructionCapabilities::ALL)
    
    bool hasTypeId();
    bool hasResultId();
    
    void setWordCount(u16);
    bool setTypeId(u32);
    bool setResultId(u32);
    void setOperands(u32* operands,u16 operand_count)
    
    u16 getWordCount();
    u16 getWordSize();
    u32 * getWords();
    
    ~InstructionType();
private:
    bool hastypeid;
    bool hasresultid;
    u16 word_count;
    u32* words; //word_count in length
}

namespace Instructions{
public:
    static InstructionType OpNop(1,0,0,false,false);
    static InstructionType OpUndef(2,45,0,true,true);
    
    static InstructionType OpSource(3,1,2,false,false);
    static InstructionType OpSourceExtension(1,2,false,false);// at least one
    static InstructionType OpName(2,54,false,false); //at least 2
    static InstructionType OpMemberName(3,55,true,false); //at least 3
    static InstructionType OpString(2,56,false,true); //at least 2
    static InstructionType OpLine(5,57,false,false);
    
    static InstructionType OpDecorationGroup(2,49,false,true);
    static InstructionType OpDecorate(3,50,false,false); //at least 3
    static InstructionType OpMemberDecorate(4,51,false,false);//at least 4
    static InstructionType OpGroupDecorate(2,52,false,false);//at least 2
    
    static InstructionType OpExtension(1,3,false,false);//at least 1
    static InstructionType OpExtInstImport(2,4,false,true);//at least 2
    static InstructionType OpExtInst(5,44,true,true); //at least 5
    
    static InstructionType OpMemoryModel(3,5,false,false);
    static InstructionType OpEntryPoint(3,6,false,false);
    static InstructionType OpExecutionMode(3,7,false,false);//at least 3
    static InstructionType OpCompileFlag(1,218,false,false,InstructionCapabilities::Kernel);//at least 1
    
    static InstructionType OpTypeVoid(2,8,false,true);
    static InstructionType OpTypeBool(2,9,false,true);
    static InstructionType OpTypeInt(4,10,false,true);
    static InstructionType OpTypeFloat(3,11,false,true);
    static InstructionType OpTypeVector(4,12,false,true);
    static InstructionType OpTypeMatrix(4,13,false,true,InstructionCapabilities::Matrix);
    static InstructionType OpTypeSampler(8,14,false,true);//at least 8
    static InstructionType OpTypeFilter(2,15,false,true);
    static InstructionType OpTypeArray(4,16,false,true);
    static InstructionType OpTypeRuntimeArray(3,17,false,true,InstructionCapabilities::Shader);
    static InstructionType OpTypeStruct(2,18,false,true);//at least 2
    static InstructionType OpTypeOpaque(2,19,false,true);//at least 2
    static InstructionType OpTypePointer(4,20,false,true);
    static InstructionType OpTypeFunction(3,21,false,true);//at least 3
    static InstructionType OpTypeEvent(2,22,false,true,InstructionCapabilities::Kernel);
    static InstructionType OpTypeDeviceEvent(2,23,false,true,InstructionCapabilities::Kernel);
    static InstructionType OpTypeReserveId(2,24,false,true,InstructionCapabilities::Kernel);
    static InstructionType OpTypeQueue(2,25,false,true,InstructionCapabilities::Kernel);
    static InstructionType OpTypePipe(4,26,false,true,InstructionCapabilities::Kernel);
    
    static InstructionType OpConstantTrue(3,27,true,true);
    static InstructionType OpConstantFalse(3,28,true,true);
    static InstructionType OpConstant(3,29,true,true);//at least 3
    static InstructionType OpConstantComposite(3,30,true,true);//at least 3
    static InstructionType OpConstantSampler(6,31,true,true,InstructionCapabilities::Kernel);
    static InstructionType OpConstantNullPointer(3,32,true,true,InstructionCapabilities::Addr);
    static InstructionType OpConstantNullObject(3,33,true,true,InstructionCapabilities::Kernel);
    static InstructionType OpSpecConstantTrue(3,34,true,true,InstructionCapabilities::Shader);
    static InstructionType OpSpecConstantFalse(3,35,true,true,InstructionCapabilities::Shader);
    static InstructionType OpSpecConstant(3,36,true,true);//at least 3
    static InstructionType OpSpecConstantComposite(3,37,true,true);//at least 3
    
    static InstructionType OpVariable(4,38,true,true);//at least 4
    static InstructionType OpVariableArray(5,39,true,true);
    static InstructionType OpLoad(4,46,true,true);//at least 4
    static InstructionType OpStore(3,47,false,false);//at least 3
    static InstructionType OpCopyMemory(3,65,false,false);//at least 3
    static InstructionType OpCopyMemorySized(4,66,false,false);//at least 4
    static InstructionType OpAccessChain(4,93,true,true);//at least 4
    static InstructionType OpInBoundsAccessChain(4,94,true,true);//at least 4
    static InstructionType OpArrayLength(5,121,true,true);
    static InstructionType OpImagePointer(6,190,true,true);
    static InstructionType OpGenericPtrMemSemantics(4,233,true,true);
    
    static InstructionType OpFunction(5,40,true,true);
    static InstructionType OpFunctionParameter(3,41,true,true);
    static InstructionType OpFunctionEnd(1,42,false,false);
    static Instructiontype OpFunctionCall(4,43,true,true);
    
    static InstructionType OpSampler(5,67,true,true);
    static InstructionType OpTextureSample(5,68,true,true,InstructionCapabilities::Shader);//at least 5
    static InstructionType OpTextureSampleDref(6,69,true,true,InstructionCapabilities::Shader);
    static InstructionType OpTextureSampleLod(6,70,true,true,InstructionCapabilities::Shader);
    static InstructionType OpTextureSampleProj(5,71,true,true,InstructionCapabilities::Shader);//at least 5
    static InstructionType OpTextureSampleGrad(7,72,true,true,InstructionCapabilities::Shader);
    static InstructionType OpTextureSampleOffset(6,73,true,true,InstructionCapabilities::Shader);//at least 6
    static InstructionType OpTextureSampleProjLod(6,74,true,true,InstructionCapabilities::Shader);
    static InstructionType OpTextureSampleProjGrad(7,75,true,true,InstructionCapabilities::Shader);
    static InstructionType OpTextureSampleLodOffset(7,76,true,true,InstructionCapabilities::Shader);
    static InstructionType OpTextureSampleProjOffset(6,77,true,true,InstructionCapabilities::Shader);//at least 6
    static InstructionType OpTextureSampleGradOffset(8,78,true,true,InstructionCapabilities::Shader);
    static InstructionType OpTextureSampleProjLodOffset(7,79,true,true,InstructionCapabilities::Shader);
    static InstructionType OpTextureSampleProjGradOffset(8,80,true,true,InstructionCapabilities::Shader);
    static InstructionType OpTextureFetchTexel(6,81,true,true,InstructionCapabilities::Shader);
    static InstructionType OpTextureFetchTexelOffset(6,82,true,true,InstructionCapabilities::Shader);
    static InstructionType OpTextureFetchSample(6,83,true,true,InstructionCapabilities::Shader);
    static InstructionType OpTextureFetchBuffer(5,84,true,true,InstructionCapabilities::Shader);
    static InstructionType OpTextureGather(6,85,true,true,InstructionCapabilities::Shader);
    static InstructionType OpTextureGatherOffset(7,86,true,true,InstructionCapabilities::Shader);
    static InstructionType OpTextureGatherOffsets(7,87,true,true,InstructionCapabilities::Shader);
    static InstructionType OpTextureQuerySizeLod(5,88,true,true,InstructionCapabilities::Shader);
    static InstructionType OpTextureQuerySize(4,89,true,true,InstructionCapabilities::Shader);
    static InstructionType OpTextureQueryLod(5,90,true,true,InstructionCapabilities::Shader);
    static InstructionType OpTextureQueryLevels(4,91,true,true,InstructionCapabilities::Shader);
    static InstructionType OpTextureQuerySamples(4,92,true,true,InstructionCapabilities::Shader);
    
    
}

}

}