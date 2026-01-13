// MyFirstGame.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "framework.h"
#include "Main.h"
#include "Engine/Direct3D.h"
#include "Engine/Camera.h"
#include "Engine/Transform.h"
#include "Engine/Input.h"
#include "Engine/RootJob.h"
#include "Engine/Model.h"
#include "Stage.h"

#pragma comment(lib, "winmm.lib")

HWND hWnd = nullptr; // ウィンドウハンドル…ウィンドウを識別するための番号　車のナンバーみたいなもん　IDとかそこらへん

#define MAX_LOADSTRING 100

// グローバル変数の宣言
const wchar_t* WIN_CLASS_NAME = L"SANPLE GAME WINDOW";
const int WINDOW_WIDTH = 800;  //ウィンドウの幅
const int WINDOW_HEIGHT = 600; //ウィンドウの高さ // SVGAサイズ

RootJob* pRootJob = nullptr; //隠せなくもないけどグローバル変数で作る

// グローバル変数:
HINSTANCE hInst;                                // 現在のインターフェイス
WCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
WCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    DlgProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    ManuProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,// hInstance：実行中のアプリケーション（プロセス）のインスタンスハンドル（識別子） 
                     _In_opt_ HINSTANCE hPrevInstance, // HINSTANCE：16ビットWindowsで使用された。特に意味はない
                     _In_ LPWSTR    lpCmdLine, // pCmdLine ：コマンド ライン引数が Unicode 文字列として含まれる
                     _In_ int       nCmdShow) // nCmdShow：メイン アプリケーション ウィンドウが最小化、最大化、または正常に表示されるかどうかを示すフラグ
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: ここにコードを挿入してください。

    // グローバル文字列を初期化する
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MYFIRSTGAME, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // アプリケーション初期化の実行:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HRESULT hr;
    hr = Direct3D::Initialize(WINDOW_WIDTH, WINDOW_HEIGHT, hWnd);
    if (FAILED(hr))
    {
        MessageBox(nullptr, L"ダイレクトxのイニシャライズに失敗しました", L"エラー", MB_OK);
        return 0;
    }

    //DirectInputの初期化
    Input::Initialize(hWnd); // 入力の初期化処理

    Camera::Initialize(); // カメラの初期化処理

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MYFIRSTGAME));

    MSG msg{};

    pRootJob = new RootJob(nullptr);
    pRootJob->Initialize();

    HWND hDig = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG2), hWnd, ManuProc, 0);
    ShowWindow(hDig, SW_SHOW);

    // メイン メッセージ ループ:
    // メッセージループは、アプリケーションがシステムからメッセージを受け取り、処理するための仕組み。
    // ユーザー操作（クリックやキー入力など）を受け取り、処理を続ける仕組み 
//    while (GetMessage(&msg, nullptr, 0, 0))
    while (msg.message != WM_QUIT)
    {
       /* if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        */
        //メッセージあり

        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {

            TranslateMessage(&msg);
            DispatchMessage(&msg);

        }

        //メッセージなし
        else
        {
            timeBeginPeriod(1);
            static DWORD countFps = 0; // FPS計測用カウンタ

            static DWORD startTime = timeGetTime(); // 初回の時間を保存
            DWORD nowTime = timeGetTime(); // 現在の時間を取得
            
            static DWORD lastUpdateTime = nowTime;
            timeEndPeriod(1);

            if (nowTime - startTime >= 1000)
            {
                string str = "FPS:" + std::to_string(nowTime - startTime) + ", " + std::to_string(countFps);
                SetWindowTextA(hWnd, str.c_str());

                countFps = 0;
                startTime = nowTime;
            }

            if (nowTime - lastUpdateTime <= 1000.0f / 60)
            {
                continue;
            }
            lastUpdateTime = nowTime;

            countFps++;
            //startTime = nowTime;

            //ゲームの処理
            Camera::Update();

            //入力情報の更新
            Input::Update();

            pRootJob->UpdateSub();

            //背景の色
            //float clearColor[4] = { 0.0f, 0.5f, 0.5f, 1.0f };//R,G,B,A
            
            //画面をクリア
            Direct3D::BeginDraw();

            //pRootJobから、全てのオブジェクトの描画をする
            pRootJob->DrawSub();

            if (Input::IsKey(DIK_D))
            {
                HRESULT hr = DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, DlgProc);
                if (hr == IDOK)
                {
                    PostQuitMessage(0);
                }
            }

            //スワップ（バックバッファを表に表示する）
            Direct3D::EndDraw();        
        }
    }

    Model::Release();
    pRootJob->ReleaseSub();
    Input::Release();
    Direct3D::Release();

    return (int) msg.wParam;
}



//
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX); // 構造体のサイズ（バイト数）を表す

    wcex.style          = CS_HREDRAW | CS_VREDRAW; // クラスのスタイル
    wcex.lpfnWndProc    = WndProc; // 「ウィンドウプロシージャ」のポインタ
    wcex.cbClsExtra     = 0; // ウィンドウクラス構造体の後に割り当てる余分なバイト数
    wcex.cbWndExtra     = 0; // ウィンドウ インスタンスの後に割り当てる余分なバイト数 
    wcex.hInstance      = hInstance; // アプリケーションのインスタンスハンドル
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYFIRSTGAME)); // クラスアイコンへのハンドル 
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW); // クラスカーソルへのハンドル 
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1); // クラスの背景ブラシへのハンドル 
    //wcex.hbrBackground = CreateSolidBrush(RGB(124, 252, 0)); // ウィンドウの色変えた
    wcex.lpszMenuName = NULL; // リソース ファイルに名前が表示されるように、クラス メニューのリソース名を指定する null で終わる文字列へのポインター 
    wcex.lpszClassName  = szWindowClass; // lpszClassName はnullで終わる文字列へのポインター、またはアトム
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL)); // 上記で設定したウィンドウクラス情報をWindowsに登録

    return RegisterClassExW(&wcex);
}

//
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // グローバル変数にインスタンス ハンドルを格納する

   //ウィンドウサイズの計算
   RECT winRect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
   AdjustWindowRect(&winRect, WS_OVERLAPPEDWINDOW, FALSE);
   int winW = winRect.right - winRect.left;     //ウィンドウ幅
   int winH = winRect.bottom - winRect.top;     //ウィンドウ高さ

   hWnd = CreateWindowW(szWindowClass, WIN_CLASS_NAME, WS_OVERLAPPEDWINDOW,
       CW_USEDEFAULT, 0, winW, winH, nullptr, nullptr, hInstance, nullptr); // ウィンドウ作る　OSが勝手に番号つける　消すときこの番号必要になるから残しとく必要がある
   //HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
   // 800,600, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
   //画像サイズ変えた
 /*  dwExStyle：作成されるウィンドウの拡張ウィンドウ スタイル

       lpClassName：ウィンドウクラス名（登録したクラス名）

       lpWindowName：ウィンドウのタイトルバーに表示される文字列

       DwStyle：ウィンドウスタイル

       X : ウィンドウの最初の水平位置。 重なったウィンドウまたはポップアップ ウィンドウの場合、x パラメーターは、ウィンドウの左上隅の最初の x 座標

       Y : ウィンドウの最初の垂直方向の位置。 重なったウィンドウまたはポップアップ ウィンドウの場合、y パラメーターは、ウィンドウの左上隅の初期 y 座標

       nWidth : ウィンドウの幅

       nHeight : ウィンドウの高さ

       HWndParent：親ウィンドウのハンドル

       HMenu：メニューのハンドルまたは子ウィンドウの識別子

       HInstance：ウィンドウに関連付けるモジュールのインスタンスへのハンドル

       lpParam：WM_CREATE メッセージの lParam パラメーターが指す CREATESTRUCT 構造体(lpCreateParams メンバー) を介してウィンドウに渡される値へのポインター。
               作成時にウィンドウプロシージャに渡す追加パラメータ

       HWND：戻り値は新しいウィンドウへのハンドル*/

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND  - アプリケーション メニューの処理
//  WM_PAINT    - メイン ウィンドウを描画する
//  WM_DESTROY  - 中止メッセージを表示して戻る
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//コールバック関数は、特定のイベントが発生した際にシステムから呼び出される関数。ウィンドウに届いたメッセージ（通知）を処理する関数 
//通知の内容として、具体的にいうとマウス動いたぞーとか今起きたぞーっていう通知、ウィンドウ作ったとかも含め何かしらあったらそれを送るのがウィンドウプロシージャー
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 選択されたメニューの解析:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break; 
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: HDC を使用する描画コードをここに追加してください...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_MOUSEMOVE:
    {
        //Input::SetMousePosition(LOWORD(lParam), HIWORD(lParam));
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);
        Input::SetMousePosition(x, y);
        OutputDebugStringA((std::to_string(x) + "," + std::to_string(y) + "\n").c_str());
        //return 0;
    }
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// バージョン情報ボックスのメッセージ ハンドラーです。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

INT_PTR CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return ((Stage*)pRootJob->FindObject("Stage"))->localProck(hWnd, message, wParam, lParam);
}

INT_PTR CALLBACK ManuProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return ((Stage*)pRootJob->FindObject("Stage"))->manuProck(hWnd, message, wParam, lParam);
}