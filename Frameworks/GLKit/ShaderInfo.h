//******************************************************************************
//
// Copyright (c) 2015 Microsoft Corporation. All rights reserved.
//
// This code is licensed under the MIT License (MIT).
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
//******************************************************************************

#pragma once

#import <Foundation/NSDictionary.h>
#import <GLKit/GLKit.h>
#import <GLKit/GLKShader.h>

#include <assert.h>
#include <map>
#include <vector>

std::string getTypeStr(GLKShaderVarType t);

// Information about a variable used in the shader program.  Basically its name, type, and location.
struct VarInfo {
    VarInfo() : type(GLKS_INVALID), loc(-1), vertexAttr(false), intermediate(false), used(false) {}
    explicit VarInfo(GLKShaderVarType vt) : type(vt), loc(-1), vertexAttr(false), intermediate(false), used(false) {}

    // when in a layout, used for variable location.
    // when in a shader mat, used for constant location in the array.
    GLKShaderVarType type;
    int loc;
    bool vertexAttr;
    bool intermediate;
    bool used;

    inline std::string getTypeStr() const { return ::getTypeStr(type); }
    inline bool isTexture() const { return (type == GLKS_SAMPLER2D || type == GLKS_SAMPLERCUBE); }
};

// Lists vertex attributes and shader variables for the vertex/pixel shader pair.
struct ShaderLayout {
    std::map<std::string, VarInfo> vars;

    inline GLKShaderVarType findVariable(const std::string& name, int* loc = NULL) {
        auto it = vars.find(name);
        if (it == vars.end()) return GLKS_INVALID;

        it->second.used = true;
        if(loc) *loc = it->second.loc;

        return it->second.type;
    }

    // Define shader variable, can be either a vertex attribute or a shader constant.  These are used
    // to define shader variables not related to the material, such as the vertex layout and the MVP matrix.
    inline void defVariable(const std::string& var, GLKShaderVarType type, int loc, bool attr = false) {
        if (type == GLKS_INVALID) return;
        if (vars.find(var) != vars.end()) return;

        VarInfo v;
        v.type = type;
        v.loc = loc;
        v.vertexAttr = attr;
        vars[var] = v;
    }

    // Convenience functions for the above.
    inline void defVertexAttr(const std::string& var)                    { defVariable(var, GLKS_FLOAT4, -1, true); }
    inline void defVertexAttr3(const std::string& var)                   { defVariable(var, GLKS_FLOAT3, -1, true); }
    inline void defVertexAttr2(const std::string& var)                   { defVariable(var, GLKS_FLOAT2, -1, true); }
    inline void defMatrix(const std::string& var)                        { defVariable(var, GLKS_MAT4, -1, false); }
};

// A shader material is just a list of shader variables and their values.  Materials can be over-specified, but
// generally aren't.
struct ShaderMaterial : public ShaderLayout {
    std::vector<float> values;
    std::map<std::string, unsigned int> inputVars;

    // These define the shader material variables.  These are just arbitrary shader variables and their values
    // coalesced into a single object.  Shader materials control shader generation in that if a value is present in
    // the material, it can be used by the shader, and if it is not, the code using it can be eliminated from the
    // shader entirely.
    void addMaterialVar(const std::string& var, GLKShaderVarType type, float* data);
    inline void addMaterialVar(const std::string& var, const GLKVector4& vec)    { addMaterialVar(var, GLKS_FLOAT4, (float*)&vec); }
    inline void addMaterialVar(const std::string& var, const GLKVector3& vec)    { addMaterialVar(var, GLKS_FLOAT3, (float*)&vec); }
    inline void addMaterialVar(const std::string& var, const GLKVector2& vec)    { addMaterialVar(var, GLKS_FLOAT2, (float*)&vec); }
    inline void addMaterialVar(const std::string& var, float val)                { addMaterialVar(var, GLKS_FLOAT, &val); }

    // Adds a float4 as a float3 for convenience.
    inline void addMaterialVar3(const std::string& var, const GLKVector4& vec)   { addMaterialVar(var, GLKS_FLOAT3, (float*)&vec); }

    // Add texture references to the shader material.
    void addTexture(const std::string& var, GLuint name, GLKShaderVarType type = GLKS_SAMPLER2D);
    inline void addTexCube(const std::string& var, GLuint name)          { addTexture(var, name, GLKS_SAMPLERCUBE); }

    // Adds an input variable to the shader material.  These can alter shader generation, but are not used
    // by the shader itself.
    inline void addInputVar(const std::string& var, unsigned int val)    { inputVars[var] = val; }
    
    inline void reset() {
        vars.clear();
        values.resize(0);
    }
};