for($i = 0; $i -lt 3; $i++)
{
    "$Env:VULKAN_SDK\Bin\glslangValidator.exe -V example$i.comp -o example$i.spv" | Invoke-Expression
}


