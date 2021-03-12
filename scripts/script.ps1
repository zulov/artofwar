$startedAt = [datetime]::UtcNow

#[DateTime]::UtcNow.ToString('HH:mm:ss.fff')
Set-Location -Path .. -PassThru
$commands = 
{powershell "& './art_of_war.exe' -headless -outputtype score -benchmarkmode -trainmode -nothreads "> logs/logfile0.txt 2>&1 }, 
{powershell "& './art_of_war.exe' -headless -outputtype score -benchmarkmode -trainmode -nothreads "> logs/logfile1.txt 2>&1 }, 
{powershell "& './art_of_war.exe' -headless -outputtype score -benchmarkmode -trainmode -nothreads "> logs/logfile2.txt 2>&1 }, 
{powershell "& './art_of_war.exe' -headless -outputtype score -benchmarkmode -trainmode -nothreads "> logs/logfile3.txt 2>&1 }, 
{powershell "& './art_of_war.exe' -headless -outputtype score -benchmarkmode -trainmode -nothreads "> logs/logfile4.txt 2>&1 }, 
{powershell "& './art_of_war.exe' -headless -outputtype score -benchmarkmode -trainmode -nothreads "> logs/logfile5.txt 2>&1 }, 
{powershell "& './art_of_war.exe' -headless -outputtype score -benchmarkmode -trainmode -nothreads "> logs/logfile6.txt 2>&1 }, 
{powershell "& './art_of_war.exe' -headless -outputtype score -benchmarkmode -trainmode -nothreads "> logs/logfile7.txt 2>&1 }, 
{powershell "& './art_of_war.exe' -headless -outputtype score -benchmarkmode -trainmode -nothreads "> logs/logfile8.txt 2>&1 }, 
{powershell "& './art_of_war.exe' -headless -outputtype score -benchmarkmode -trainmode -nothreads "> logs/logfile9.txt 2>&1 }, 

{powershell "& './art_of_war.exe' -headless -outputtype score -benchmarkmode -trainmode -nothreads "> logs/logfile10.txt 2>&1 }, 
{powershell "& './art_of_war.exe' -headless -outputtype score -benchmarkmode -trainmode -nothreads "> logs/logfile11.txt 2>&1 }, 
{powershell "& './art_of_war.exe' -headless -outputtype score -benchmarkmode -trainmode -nothreads "> logs/logfile12.txt 2>&1 }, 
{powershell "& './art_of_war.exe' -headless -outputtype score -benchmarkmode -trainmode -nothreads "> logs/logfile13.txt 2>&1 }, 
{powershell "& './art_of_war.exe' -headless -outputtype score -benchmarkmode -trainmode -nothreads "> logs/logfile14.txt 2>&1 }, 
{powershell "& './art_of_war.exe' -headless -outputtype score -benchmarkmode -trainmode -nothreads "> logs/logfile15.txt 2>&1 }, 
{powershell "& './art_of_war.exe' -headless -outputtype score -benchmarkmode -trainmode -nothreads "> logs/logfile16.txt 2>&1 }, 
{powershell "& './art_of_war.exe' -headless -outputtype score -benchmarkmode -trainmode -nothreads "> logs/logfile17.txt 2>&1 }, 
{powershell "& './art_of_war.exe' -headless -outputtype score -benchmarkmode -trainmode -nothreads "> logs/logfile18.txt 2>&1 }, 
{powershell "& './art_of_war.exe' -headless -outputtype score -benchmarkmode -trainmode -nothreads "> logs/logfile19.txt 2>&1 }, 

{powershell "& './art_of_war.exe' -headless -outputtype score -benchmarkmode -trainmode -nothreads "> logs/logfile20.txt 2>&1 }, 
{powershell "& './art_of_war.exe' -headless -outputtype score -benchmarkmode -trainmode -nothreads "> logs/logfile21.txt 2>&1 }, 
{powershell "& './art_of_war.exe' -headless -outputtype score -benchmarkmode -trainmode -nothreads "> logs/logfile22.txt 2>&1 }, 
{powershell "& './art_of_war.exe' -headless -outputtype score -benchmarkmode -trainmode -nothreads "> logs/logfile23.txt 2>&1 }, 
{powershell "& './art_of_war.exe' -headless -outputtype score -benchmarkmode -trainmode -nothreads "> logs/logfile24.txt 2>&1 }, 
{powershell "& './art_of_war.exe' -headless -outputtype score -benchmarkmode -trainmode -nothreads "> logs/logfile25.txt 2>&1 }, 
{powershell "& './art_of_war.exe' -headless -outputtype score -benchmarkmode -trainmode -nothreads "> logs/logfile26.txt 2>&1 }, 
{powershell "& './art_of_war.exe' -headless -outputtype score -benchmarkmode -trainmode -nothreads "> logs/logfile27.txt 2>&1 }, 
{powershell "& './art_of_war.exe' -headless -outputtype score -benchmarkmode -trainmode -nothreads "> logs/logfile28.txt 2>&1 }, 

{powershell "& './art_of_war.exe' -headless -outputtype score -benchmarkmode -trainmode -nothreads" > logs/logfileX.txt 2>&1 }


$jobs = $commands | Foreach-Object { Start-ThreadJob $_ }

#[DateTime]::UtcNow.ToString('HH:mm:ss.fff')

# Wait until all jobs have completed.
#$null = Wait-Job $jobs
$jobs | Receive-Job -Wait -AutoRemoveJob
Set-Location -Path scripts -PassThru
#[DateTime]::UtcNow.ToString('HH:mm:ss.fff')
"`nAll jobs completed. Total runtime in secs.: $('{0:N2}' -f ([datetime]::UtcNow - $startedAt).TotalSeconds)"
