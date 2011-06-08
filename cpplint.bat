for %%c in (*.h, *.cpp) do cpplint.py --output=vs7 --counting=detailed %%c
pause
