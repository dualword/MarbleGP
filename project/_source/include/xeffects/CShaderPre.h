#ifndef H_C_SHADER_PP
#define H_C_SHADER_PP

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

class CShaderPreprocessor
{
public:
	CShaderPreprocessor(irr::video::IVideoDriver* driverIn, irr::IrrlichtDevice *a_pDevice);
	irr::core::stringc ppShader(irr::core::stringc shaderProgram);
	irr::core::stringc ppShaderFF(irr::core::stringc shaderProgram);
	void addShaderDefine(const irr::core::stringc name, const irr::core::stringc value = "");
	void removeShaderDefine(const irr::core::stringc name);

private:
	void initDefineMap();

	irr::video::IVideoDriver* driver;
  irr::IrrlichtDevice *m_pDevice;
	irr::core::map<irr::core::stringc , irr::core::stringc> DefineMap;
};

#endif