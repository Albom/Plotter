
Dim As String cmd, answer
Dim As String port

Open "settings.txt" For Input As #1
Input #1, port
Close #1

Open "gcode.txt" For Input As #1

If Open Com (port+":9600,n,8,1,cs,rs,ds,bin" For Binary As #2) <> 0 Then
	Print "Unable to open serial port"
	Sleep
	End
End If

Print "Please reset plotter. Waiting for 'start' signal... ";
Do
	Input #2, answer
	Sleep(100)
Loop While answer <> "start"
Print "OK"

Print "Begin..."

While Not Eof(1)

	Input #1, cmd
	Print cmd,,
	Print #2, cmd + Chr(13, 10);
	Sleep(500)

	Do
		Sleep(100)
		Input #2, answer
	Loop While answer <> "OK"
	Print answer

Wend

Close #1
Close #2

Print
Print "End."
Sleep
End
