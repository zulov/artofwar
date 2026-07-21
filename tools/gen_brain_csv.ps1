<#
.SYNOPSIS
    Brain CSV generator for Art of War.

.DESCRIPTION
    Emits a feed-forward brain weight file in the exact on-disk format the C++
    loader (game/src/utils/FileUtils.h + nn/Layer.cpp) expects.

    Brain definitions are passed in with -Brains using this format:

      Name|Input|Hidden1,Hidden2,...|Output

    Example:

      -Brains 'master.csv|34|22|9','economy.csv|41|28|18'

      Line 1 (input holder): <inputSize weights> ;; <1 bias>
          Its weights/bias are ignored at runtime (Brain::decide only calls
          setInput() on layer 0), but its COLUMN COUNT declares the input size
          that the brain constructors assert against (getInputSize()).
      Line 2..N (dense layers): <in*out weights, row-major> ;; <out biases>
          Layer.cpp computes values = tanh(W * input + b), W shaped [out x in].

    Only the COUNT of weights (in*out) and biases (out) matters for correctness;
    values are random in [-1, 1] (tanh-friendly). These are UNTRAINED brains:
    they make the AI run with valid dimensions, not play well (real strength
    needs the external genetic algorithm).

.PARAMETER Brains
    Brain definitions in the form Name|Input|Hidden1,Hidden2,...|Output.

.PARAMETER Check
    Print intended topologies only; write nothing.

.PARAMETER Seed
    RNG seed for reproducibility (default 1337).

.PARAMETER OutDir
    Output directory (default: game/Data/ai relative to repo root).

.EXAMPLE
    powershell -ExecutionPolicy Bypass -File tools\gen_brain_csv.ps1 -Brains 'military.csv|27|16|24','build_spatial.csv|20|12|11'
.EXAMPLE
    powershell -ExecutionPolicy Bypass -File tools\gen_brain_csv.ps1 -Check -Brains 'military.csv|27|16|24','build_spatial.csv|20|12|11'
#>
[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string[]]$Brains,

    [switch]$Check,
    [int]$Seed = 1337,
    [string]$OutDir
)

$ErrorActionPreference = 'Stop'

if (-not $OutDir) {
    $repoRoot = Split-Path -Parent $PSScriptRoot
    $OutDir = Join-Path $repoRoot 'game\Data\ai'
}

$rng = [System.Random]::new($Seed)

function New-Rand {
    # uniform in [-1, 1]
    return ($rng.NextDouble() * 2.0) - 1.0
}

function Format-Values([double[]]$values) {
    return ($values | ForEach-Object { $_.ToString('F6', [System.Globalization.CultureInfo]::InvariantCulture) }) -join ';'
}

function Parse-BrainSpec([string]$spec) {
    $parts = $spec -split '\|', 4
    if ($parts.Count -ne 4) {
        throw "Invalid brain spec '$spec'. Expected format: Name|Input|Hidden1,Hidden2,...|Output"
    }

    $hidden = @()
    if (-not [string]::IsNullOrWhiteSpace($parts[2])) {
        $hidden = @($parts[2].Split(',') | ForEach-Object {
            $value = $_.Trim()
            if ($value.Length -eq 0) {
                return
            }
            [int]$value
        } | Where-Object { $null -ne $_ })
    }

    return [pscustomobject]@{
        Name = $parts[0].Trim()
        Input = [int]$parts[1]
        Hidden = $hidden
        Output = [int]$parts[3]
    }
}

function Build-File([int]$inputSize, [int[]]$hidden, [int]$outputSize) {
    $sizes = @($inputSize) + $hidden + @($outputSize)
    $lines = New-Object System.Collections.Generic.List[string]

    # Line 1: input holder (inputSize weights, 1 bias = 0).
    $holderWeights = @(for ($i = 0; $i -lt $inputSize; $i++) { New-Rand })
    $lines.Add((Format-Values $holderWeights) + ';;' + (Format-Values @(0.0)))

    # Dense layers.
    for ($li = 0; $li -lt $sizes.Count - 1; $li++) {
        $inDim = $sizes[$li]
        $outDim = $sizes[$li + 1]
        $weights = @(for ($i = 0; $i -lt ($inDim * $outDim); $i++) { New-Rand })
        $biases = @(for ($i = 0; $i -lt $outDim; $i++) { New-Rand })
        $lines.Add((Format-Values $weights) + ';;' + (Format-Values $biases))
    }

    return ($lines -join "`n") + "`n"
}

foreach ($b in ($Brains | ForEach-Object { Parse-BrainSpec $_ })) {
    $topo = (@($b.Input) + $b.Hidden + @($b.Output)) -join ' -> '
    if ($Check) {
        Write-Output ("{0,-22} input-holder({1}) + {2}" -f $b.Name, $b.Input, $topo)
        continue
    }
    $content = Build-File $b.Input $b.Hidden $b.Output
    $path = Join-Path $OutDir $b.Name
    # Write without BOM, LF line endings.
    [System.IO.File]::WriteAllText($path, $content, (New-Object System.Text.UTF8Encoding($false)))
    Write-Output ("wrote {0}  ({1})" -f $path, $topo)
}
