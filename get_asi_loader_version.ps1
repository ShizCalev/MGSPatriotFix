# Reads ProductVersion from dist\winhttp.dll and updates src\resources\version.h.
# Fails hard if DLL or version.h is missing.

Write-Host "=== Getting Version Number from ASI Loader dll ==="

# Only run in CI
if ($env:CI -ne "true") {
    Write-Host "[ASI Loader] CI environment variable not set to 'true'; skipping ASI loader version update."
	Write-Host "=== Skipped getting version number from ASI Loader dll ==="
    exit 0
}

$ErrorActionPreference = "Stop"

$dllPath    = Join-Path $PSScriptRoot "dist\winhttp.dll"
$headerPath = Join-Path $PSScriptRoot "src\resources\version.h"

# ------------------------------------------------------------
# Helpers: read/write while preserving encoding + BOM
# ------------------------------------------------------------
function Get-FileTextAndEncoding {
    param([Parameter(Mandatory=$true)][string]$Path)

    $fs = [System.IO.File]::Open($Path, 'Open', 'Read', 'Read')
    try {
        $sr = New-Object System.IO.StreamReader($fs, $true)
        try {
            $text = $sr.ReadToEnd()
            $enc  = $sr.CurrentEncoding
            return ,@($text, $enc)
        } finally {
            $sr.Dispose()
        }
    } finally {
        $fs.Dispose()
    }
}

function Write-FileTextPreserveBom {
    param(
        [Parameter(Mandatory=$true)][string]$Path,
        [Parameter(Mandatory=$true)][string]$Text,
        [Parameter(Mandatory=$true)][System.Text.Encoding]$Encoding
    )

    $originalBytes = [System.IO.File]::ReadAllBytes($Path)
    $preamble = $Encoding.GetPreamble()
    $hadBom = $false

    if ($preamble.Length -gt 0 -and $originalBytes.Length -ge $preamble.Length) {
        $hadBom = $true
        for ($i = 0; $i -lt $preamble.Length; $i++) {
            if ($originalBytes[$i] -ne $preamble[$i]) {
                $hadBom = $false
                break
            }
        }
    }

    $payload = $Encoding.GetBytes($Text)

    if ($hadBom -and $preamble.Length -gt 0) {
        $out = New-Object byte[] ($preamble.Length + $payload.Length)
        [Array]::Copy($preamble, 0, $out, 0, $preamble.Length)
        [Array]::Copy($payload, 0, $out, $preamble.Length, $payload.Length)
        [System.IO.File]::WriteAllBytes($Path, $out)
    } else {
        if ($Encoding.WebName -eq "utf-8") {
            $utf8NoBom = New-Object System.Text.UTF8Encoding($false)
            [System.IO.File]::WriteAllBytes($Path, $utf8NoBom.GetBytes($Text))
        } else {
            [System.IO.File]::WriteAllBytes($Path, $payload)
        }
    }
}

# ------------------------------------------------------------
# Hard-fail if required files are missing
# ------------------------------------------------------------
if (-not (Test-Path -LiteralPath $dllPath)) {
    Write-Error "[ASI Loader] ERROR: Required DLL not found: $dllPath"
    exit 1
}

if (-not (Test-Path -LiteralPath $headerPath)) {
    Write-Error "[ASI Loader] ERROR: version header not found: $headerPath"
    exit 1
}

# ------------------------------------------------------------
# Extract ProductVersion
# ------------------------------------------------------------
try {
    $version = (Get-Item -LiteralPath $dllPath).VersionInfo.ProductVersion
    if ([string]::IsNullOrWhiteSpace($version)) {
        throw "ProductVersion is empty"
    }
    $version = $version.Trim()
} catch {
    Write-Error "[ASI Loader] ERROR: Failed to read ProductVersion from $dllPath"
    exit 1
}

Write-Host "[ASI Loader] Using ASI_LOADER_VERSION_STRING = $version"

# ------------------------------------------------------------
# Read, replace, write
# ------------------------------------------------------------
$result   = Get-FileTextAndEncoding -Path $headerPath
$content  = $result[0]
$encoding = $result[1]

$pattern = '(?m)^(?<prefix>\s*.*\bASI_LOADER_VERSION_STRING\b\s*=\s*)("([^"\\]|\\.)*"|''([^''\\]|\\.)*'')(?<suffix>\s*;\s*(//.*)?)\s*$'

if ($content -notmatch $pattern) {
    Write-Error "[ASI Loader] ERROR: Could not find ASI_LOADER_VERSION_STRING assignment in $headerPath"
    exit 1
}

$replacement = '${prefix}"' + $version + '"${suffix}'
$newContent = [regex]::Replace($content, $pattern, $replacement)

if ($newContent -ne $content) {
    Write-FileTextPreserveBom -Path $headerPath -Text $newContent -Encoding $encoding
    Write-Host "[ASI Loader] Version updated successfully."
} else {
    Write-Host "[ASI Loader] No change needed."
}

Write-Host "=== Finished getting version number from ASI Loader dll ==="
