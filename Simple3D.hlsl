//───────────────────────────────────────
// テクスチャ＆サンプラーデータのグローバル変数定義
//───────────────────────────────────────
Texture2D g_texture : register(t0); //テクスチャー
SamplerState g_sampler : register(s0); //サンプラー

//───────────────────────────────────────
// コンスタントバッファ
// DirectX 側から送信されてくる、ポリゴン頂点以外の諸情報の定義
//───────────────────────────────────────
cbuffer global
{
    float4x4 matWVP;            // ワールド・ビュー・プロジェクションの合成行列
    float4x4 matNormalTrans;    // 法線の変換行列
    float4x4 matWorld;          // ワールド行列
    float4   vecLightDir;       // ライトの方向ベクトル
    float4   diffuseColor;      // ディフューズ色
    float4   ambientColor;      // アンビエントカラー
    float4   speculerColor;     // スペキュラーカラー
    float4   cameraPosition;    // カメラの位置
    float    shininess;         //ハイライトの強さ
    bool     useTextrue;        // テクスチャーを使うかどうか
};

//───────────────────────────────────────
// 頂点シェーダー出力＆ピクセルシェーダー入力データ構造体
//───────────────────────────────────────
struct VS_OUT
{
            // セマンティクス
    float4 pos      : SV_POSITION;  //位置
    float4 normal   : TEXCOORD2; //法線
    float2 uv       : TEXCOORD0;    //UV座標
    float4 eye      : TEXCOORD1;    //視線
};

//───────────────────────────────────────
// 頂点シェーダ
//───────────────────────────────────────
VS_OUT VS(float4 pos : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD)
{
	//ピクセルシェーダーへ渡す情報
    VS_OUT outData;
    
	//ローカル座標に、ワールド・ビュー・プロジェクション行列をかけて
	//スクリーン座標に変換し、ピクセルシェーダーへ
    outData.pos = mul(pos, matWVP);
    
    // 法線を回転
    normal.w = 0;
    normal = mul(normal, matNormalTrans);
    outData.normal = normal;
    
    //視線ベクトル
    float4 worldPos = mul(pos, matWorld); //ローカル座標にワールド行列をかけてワールド座標へ
    outData.eye = normalize(cameraPosition - worldPos); //視点から頂点位置を引き算し視線を求めてピクセルシェーダーへ
    
    outData.uv = uv; // UV座標はそのまま
    
	//まとめて出力
    return outData;
}

//───────────────────────────────────────
// ピクセルシェーダ
//───────────────────────────────────────
float4 PS(VS_OUT inData) : SV_Target
{
	//ライトの向き
    float4 lightDir = vecLightDir; //グルーバル変数は変更できないので、いったんローカル変数へ
    lightDir = normalize(lightDir); //向きだけが必要なので正規化

	//法線はピクセルシェーダーに持ってきた時点で補完され長さが変わっている
	//正規化しておかないと面の明るさがおかしくなる
    inData.normal = normalize(inData.normal);

	//拡散反射光（ディフューズ）
	//法線と光のベクトルの内積が、そこの明るさになる
    float4 shade = saturate(dot(inData.normal, -lightDir));
    shade.a = 1; //暗いところが透明になるので、強制的にアルファは1

    float4 diffuse;
	//テクスチャ有無
    if (useTextrue == true)
    {
		//テクスチャの色
        diffuse = g_texture.Sample(g_sampler, inData.uv);
    }
    else
    {
		//マテリアルの色
        diffuse = diffuseColor;
    }

	//環境光（アンビエント）
	//これはMaya側で指定し、グローバル変数で受け取ったものをそのまま
    float4 ambient = ambientColor;

	//鏡面反射光（スペキュラー）
    float4 speculer = float4(0, 0, 0, 0); //とりあえずハイライトは無しに
    if (speculerColor.a != 0)	//スペキュラーの情報があれば
    {
        float4 R = reflect(lightDir, inData.normal); //正反射ベクトル
        speculer = pow(saturate(dot(R, inData.eye)), shininess) * speculerColor; //ハイライトを求める
    }

	//最終的な色
    return diffuse * shade + diffuse * ambient;//+speculer;

    //一旦一定で仮置き
    //float4 color;
    //if (useTextrue == 1)
    //{
    //    color = g_texture.Sample(g_sampler, inData.uv); // * inData.color;
    //}
    //else
    //{
    //    color = float4(0.9, 0.7, 0.5, 1);
    //}
    
    //return color;
    
}