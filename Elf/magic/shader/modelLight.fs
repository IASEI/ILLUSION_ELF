#version 430 core

//ASES=ASE+Shader


//-----------------------------------------------------------------------------------------------------------
//
struct ASESMaterial{float shininess;vec3 vdiffuse,vspecular;sampler2D diffuse,specular;};
//平行光
struct ASESLightDirction{vec3 direction,ambient,diffuse,specular;};
//点光源
struct ASESLightPoint{float constant,linear,quadratic;vec3 position,ambient,diffuse,specular;};
//聚光灯
struct ASESLightSpot{float cutOff,outerCutOff,constant,linear,quadratic;vec3 ambient,diffuse,specular,position,direction;};
//配置
struct ASESConfig{bool isDiffuseSampler,isSpecularSampler,isLightdirction;int lightPointNum,lightSpotNum;};
//-----------------------------------------------------------------------------------------------------------

#define NR_POINT_LIGHTS 1

//光照相关函数
vec3    CalcDirLight(ASESLightDirction light, vec3 normal, vec3 carmeDir,float shadow);
vec3    CalcPointLight(ASESLightPoint light, vec3 normal, vec3 fragPos, vec3 carmeDir,float shadow);
vec3    CalcSpotLight(ASESLightSpot light, vec3 normal, vec3 fragPos, vec3 carmeDir,float shadow);
float   ShadowCalculation(vec4 fragPosLightSpace,ASESLightDirction light, vec3 normal);
//-----------------------------------------------------------------------------------------------------------

//设置变量
out     vec4                mFragColor;
in      vec2                mTexCoords;
in      vec3                mNormals;
in      vec3                mFragPos;
in      vec4                mFragPosLightSpace;

in vec3 mLightLocation;
in vec3 mLightDirection;	
in vec3 mLightAmbient;	
in vec3 mLightDiffuse;

uniform sampler2D           mShadowMap;
uniform vec3                mCarmePos;
uniform vec3                mLightPos;

uniform ASESLightPoint      mPointLight[10];
uniform ASESLightSpot       mSpotLight;
uniform ASESMaterial        mMaterial;
uniform ASESConfig          mConfig; 

void main()
{
    //标准化
    vec3  norm=normalize(mNormals);                                      //单位法向量
    vec3  carmeDir=normalize(mCarmePos-mFragPos);                        //相机向量
    vec3  result=vec3(0.0f,0.0f,0.0f);                                   //最后结果
    float gamma = 1.4;                                                   //伽马常数
    float shadow=0;                                                      //阴影结果
	ASESLightDirction   mDirLight;
	mDirLight.direction=mLightDirection ;
	mDirLight.ambient=mLightAmbient ;
	mDirLight.diffuse=mLightAmbient ;
	mDirLight.specular=mLightAmbient ;
    shadow = ShadowCalculation(mFragPosLightSpace,mDirLight,norm);       //阴影计算
    result += CalcDirLight(mDirLight,norm,carmeDir,shadow);              //平行光光照
    for(int i = 0; i < mConfig.lightPointNum; i++)                       //点光源光照
        result = CalcPointLight(mPointLight[i], norm, mFragPos, carmeDir,shadow);
    for(int i = 0; i < mConfig.lightSpotNum;i++)                         //聚光灯
        result += CalcSpotLight(mSpotLight, norm, mFragPos, carmeDir,shadow);    
    mFragColor = vec4(pow(result, vec3(1.0/gamma)), 1.0f);
}

//平行光
vec3 CalcDirLight(ASESLightDirction light, vec3 normal, vec3 carmeDir,float shadow)
{
    vec3  diffuse,specular,ambient;
    vec3  lightDir=normalize(-light.direction);                         //标准化光线
    vec3  reflectDir=reflect(-lightDir,normal);                         //技术安反射光
    float diff=max(dot(normal,lightDir),0.0);                           //漫反射强度
    float spec=pow(max(dot(carmeDir,reflectDir),0.0),mMaterial.shininess);
    //相加
    ambient=light.ambient*vec3(texture(mMaterial.diffuse,mTexCoords));
    if(!mConfig.isDiffuseSampler)
        diffuse=light.diffuse*diff*mMaterial.vdiffuse;
    else
        diffuse=light.diffuse*diff*vec3(texture(mMaterial.diffuse,mTexCoords));
    if(!mConfig.isSpecularSampler)
        specular=light.specular*spec*mMaterial.vspecular;
    else
        specular=light.specular*spec*vec3(texture(mMaterial.specular,mTexCoords));
    return (ambient+(1.0-shadow)*(diffuse+specular));
}

//点光源
vec3 CalcPointLight(ASESLightPoint light, vec3 normal, vec3 fragPos, vec3 carmeDir,float shadow)
{
    vec3 diffuse,specular,ambient;
    vec3 lightDir=normalize(light.position-mFragPos);
    float diff=max(dot(normal,lightDir),0.0);
    vec3 reflectDir=reflect(-lightDir,normal);
    float spec=pow(max(dot(carmeDir,reflectDir),0.0),mMaterial.shininess);
    //顶点与光线距离
    float distance=length(light.position-mFragPos);
    float attenuation=1.0/(light.constant+light.linear*distance+light.quadratic*(distance*distance));//光线衰弱
     ambient=light.ambient*vec3(texture(mMaterial.diffuse,mTexCoords));
    if(!mConfig.isDiffuseSampler)
        diffuse=light.diffuse*diff*mMaterial.vdiffuse;
    else
        diffuse=light.diffuse*diff*vec3(texture(mMaterial.diffuse,mTexCoords));
    if(!mConfig.isSpecularSampler)
        specular=light.specular*spec*mMaterial.vspecular;
    else
        specular=light.specular*spec*vec3(texture(mMaterial.specular,mTexCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient+(1.0-shadow)*(diffuse+specular));
}

//聚光灯
vec3 CalcSpotLight(ASESLightSpot light, vec3 normal, vec3 fragPos, vec3 carmeDir,float shadow)
{
    vec3 diffuse,specular,ambient;
    vec3 lightDir = normalize(light.position - mFragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(carmeDir, reflectDir), 0.0), mMaterial.shininess);
    float distance = length(light.position - mFragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    ambient=light.ambient*vec3(texture(mMaterial.diffuse,mTexCoords));
    if(!mConfig.isDiffuseSampler)
        diffuse=light.diffuse*diff*mMaterial.vdiffuse;
    else
        diffuse=light.diffuse*diff*vec3(texture(mMaterial.diffuse,mTexCoords));
    if(!mConfig.isSpecularSampler)
        specular=light.specular*spec*mMaterial.vspecular;
    else
        specular=light.specular*spec*vec3(texture(mMaterial.specular,mTexCoords));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient+(1.0-shadow)*(diffuse+specular));
}

//阴影
float ShadowCalculation(vec4 fragPosLightSpace,ASESLightDirction light, vec3 normal)
{
    vec3 lightDir=normalize(-light.direction);//标准化光线
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(mShadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(mShadowMap, 0);
    for(int x = -4; x <= 4; ++x)
    {
        for(int y = -4; y <= 4; ++y)
        {
            float pcfDepth = texture(mShadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 81.0;
    if(projCoords.z > 1.0)
        shadow = 0.0;
    return shadow;
}
