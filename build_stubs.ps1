# build_stubs.ps1 - One-click compile all Win7 proxy DLLs
# Run from E:\ipp-sharing\  (needs MinGW gcc in PATH)
param([switch]$Clean)

$release = "target\x86_64-pc-windows-gnu\release"

$stubs = @(
    @{DLL="api-ms-win-core-winrt-l1-1-0.dll";       Src="stubs/winrt_stub.c";        Def=$null;    Libs=""},
    @{DLL="api-ms-win-core-winrt-error-l1-1-0.dll";  Src="stubs/winrt_error_stub.c";  Def=$null;    Libs=""},
    @{DLL="api-ms-win-core-synch-l1-2-0.dll";        Src="stubs/synch_stub.c";        Def="stubs/synch_stub.def";    Libs=""},
    @{DLL="bcryptprimitives.dll";                    Src="stubs/bcrypt_stub.c";        Def="stubs/bcrypt_stub.def";   Libs=""},
    @{DLL="combase.dll";                             Src="stubs/combase_stub.c";       Def="stubs/combase_stub.def";  Libs="-lole32"},
    @{DLL="windows.data.pdf.dll";                    Src="stubs/winpdf_stub.c";        Def=$null;    Libs=""}
)

$ok = 0; $fail = 0
foreach ($s in $stubs) {
    $dll = $s.DLL
    $cmd = "gcc -shared -nostartfiles -nodefaultlibs -s -O2 -o `"$dll`" `"$($s.Src)`""
    if ($s.Def) { $cmd += " `"$($s.Def)`"" }
    if ($s.Libs) { $cmd += " $($s.Libs)" }
    $cmd += " -lkernel32"
    
    Write-Host "[$dll]" -NoNewline -ForegroundColor Cyan
    $exitCode = (Start-Process cmd -ArgumentList "/c $cmd 2>nul" -Wait -PassThru -NoNewWindow).ExitCode
    if ($exitCode -ne 0) {
        Write-Host " FAILED (exit $exitCode)" -ForegroundColor Red
        $fail++
    } else {
        Copy-Item $dll $release -Force
        Write-Host " OK $((Get-Item $dll).Length) bytes" -ForegroundColor Green
        $ok++
    }
}

Write-Host "`n=== $ok ok, $fail failed ===" -ForegroundColor $(if($fail){'Red'}else{'Cyan'})
Write-Host "`nCopy these to Win7 alongside ipp-sharing_patched.exe:" -ForegroundColor Yellow
Get-ChildItem $release -File | ?{ $_.Name -like "*.dll" -and $_.Name -notlike "pdfium*" } | %{ "  $($_.Name)" }
Write-Host "`nREMEMBER: On Win7 copy C:\Windows\System32\bcryptprimitives.dll to EXE dir as bcryptprimitives_orig.dll" -ForegroundColor Magenta
