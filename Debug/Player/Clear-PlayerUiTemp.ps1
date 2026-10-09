<#
只清理本次玩家 UI 收尾产生的临时截图
默认预览清单 传入 -Apply 才删除 使用 -WhatIf 可检查删除动作
保留 Content 源码 编译缓存 交付截图和其它会话的临时目录
#>
[CmdletBinding(SupportsShouldProcess = $true)]
param(
    [string]$ProjectRoot = 'E:\BBB_Evac',
    [switch]$Apply
)

$ErrorActionPreference = 'Stop'
$cleanupRoot = [IO.Path]::GetFullPath($ProjectRoot).TrimEnd('\')
if (-not (Test-Path -LiteralPath (Join-Path $cleanupRoot 'ABBB_Evac.uproject') -PathType Leaf))
{
    throw '指定目录不是 BBB_Evac 项目'
}

$ownedFiles = @(
    'Saved\Temp\McpScreenshots\bagclean-20261009-rifle.png',
    'Saved\Temp\McpScreenshots\bagclean-20261009-rifle-52.png',
    'Saved\Temp\McpScreenshots\bagfinal-20261009-equipment.png',
    'Saved\Temp\McpScreenshots\bagfinal-20261009-misc.png',
    'Saved\Temp\McpScreenshots\bagfinal-20261009-gear.png',
    'Saved\Temp\McpScreenshots\bagfinal-20261009-4x3.png',
    'Saved\Temp\McpScreenshots\bagfinal-20261009-ultrawide.png',
    'Saved\Temp\McpScreenshots\bagfinal-20261009-tab.png'
)

$fileCount = 0
$totalBytes = 0L
foreach ($relativePath in $ownedFiles)
{
    $targetPath = [IO.Path]::GetFullPath((Join-Path $cleanupRoot $relativePath))
    if (-not $targetPath.StartsWith(($cleanupRoot + '\Saved\Temp\'), [StringComparison]::OrdinalIgnoreCase))
    {
        throw '清理目标超出项目临时目录'
    }
    if (-not (Test-Path -LiteralPath $targetPath -PathType Leaf))
    {
        continue
    }
    $targetFile = Get-Item -LiteralPath $targetPath -Force
    $ancestor = $targetFile
    while ($null -ne $ancestor)
    {
        if (($ancestor.Attributes -band [IO.FileAttributes]::ReparsePoint) -ne 0)
        {
            throw "拒绝清理含目录链接的路径 $targetPath"
        }
        if ($ancestor -is [IO.FileInfo])
        {
            $ancestor = $ancestor.Directory
            continue
        }
        $ancestor = $ancestor.Parent
    }
    $fileCount++
    $totalBytes += $targetFile.Length
    if (-not $Apply)
    {
        Write-Output "预览 $targetPath"
        continue
    }
    if ($PSCmdlet.ShouldProcess($targetPath, '删除本次 UI 临时截图'))
    {
        Remove-Item -LiteralPath $targetPath -Force
        Write-Output "已清理 $targetPath"
    }
}

Write-Output ("本次清单 {0} 个文件 {1:N2} MB" -f $fileCount, ($totalBytes / 1MB))
