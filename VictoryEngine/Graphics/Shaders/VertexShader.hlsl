struct VSInput
{
    float3 position : POSITION;
    float4 colour : COLOR;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float4 colour : COLOR;
};

PSInput main(VSInput input)
{
    PSInput output;
    
    output.position = float4(input.position, 1.0f);
    output.colour = input.colour;

    return output;
}