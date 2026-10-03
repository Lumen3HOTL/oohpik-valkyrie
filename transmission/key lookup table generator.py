

sfkeys="Unknown , A, B , C , D , E , F , G , H , I , J , K , L , M , N , O , P , Q , R , S , T , U , V , W , X , Y , Z , Num0 , Num1 , Num2 , Num3 , Num4 , Num5 , Num6 , Num7 , Num8 , Num9 , Escape , LControl , LShift , LAlt , LSystem , RControl , RShift , RAlt , RSystem , Menu , LBracket , RBracket , Semicolon , Comma , Period , Apostrophe , Slash , Backslash , Grave ,Equal , Hyphen , Space , Enter ,Backspace , Tab , PageUp , PageDown ,End , Home , Insert , Delete ,Add , Subtract , Multiply , Divide ,Left , Right , Up , Down ,Numpad0 , Numpad1 , Numpad2 , Numpad3 ,Numpad4 , Numpad5 , Numpad6 , Numpad7 ,Numpad8 , Numpad9 , F1 , F2 ,F3 , F4 , F5 , F6 ,F7 , F8 , F9 , F10 ,F11 , F12 , F13 , F14 ,F15 , Pause"
uppersfkeys=sfkeys.upper()
sfkeys=sfkeys.split(",")
uppersfkeys=uppersfkeys.split(",")
for i,j in enumerate(sfkeys):
    sfkeys[i]=j.strip()

for i,j in enumerate(uppersfkeys):
    uppersfkeys[i]=j.strip()
    
dfkeys="UNDEFINED_KEY,SPACE, RETURN, ESCAPE, TAB, LEFTARROW, RIGHTARROW, UPARROW, DOWNARROW,PAUSE, MINUS, PLUS, TILDE, PERIOD, COMMA, SLASH, LEFTCONTROL,RIGHTCONTROL, LEFTSHIFT, RIGHTSHIFT, F1, F2, F3, F4, F5, F6, F7, F8,F9, F10, F11, F12, A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q,R, S, T, U, V, W, X, Y, Z, NUM1, NUM2, NUM3, NUM4, NUM5, NUM6, NUM7,NUM8, NUM9, NUM0, BACKSPACE, EQUAL, BACKSLASH, LEFTALT, RIGHTALT, LEFTBRACKET, RIGHTBRACKET, SEMICOLON, APOSTROPHE, HYPHEN, PAGEUP, PAGEDOWN, END, HOME, INSERT, KDELETE, STAR, NUMPAD0, NUMPAD1, NUMPAD2,NUMPAD3, NUMPAD4, NUMPAD5, NUMPAD6, NUMPAD7, NUMPAD8, NUMPAD9"
dfkeys= dfkeys.upper()
dfkeys=dfkeys.split(",")

for i,j in enumerate(dfkeys):
    dfkeys[i]=j.strip()

matches = [("UNKNOWN", "UNDEFINED_KEY"),("LCONTROL", "LEFTCONTROL"),("LSHIFT", "LEFTSHIFT"),("LALT", "LEFTALT"),("RCONTROL", "RIGHTCONTROL"),("RSHIFT", "RIGHTSHIFT"),("RALT", "RIGHTALT"),("LBRACKET", "LEFTBRACKET"),("RBRACKET", "RIGHTBRACKET"),("GRAVE","TILDE"),("ENTER", "RETURN"),("ADD", "PLUS"),("SUBTRACT", "MINUS"),("MULTIPLY", "STAR"),("DIVIDE", "SLASH"),("LEFT", "LEFTARROW"),("RIGHT", "RIGHTARROW"),("UP", "UPARROW"),("DOWN", "DOWNARROW"),("LSYSTEM", "UNDEFINED_KEY"),("RSYSTEM", "UNDEFINED_KEY"),("MENU", "UNDEFINED_KEY"),("F13", "UNDEFINED_KEY"),("F14", "UNDEFINED_KEY"),("F15", "UNDEFINED_KEY")]
matches1 = dict(matches)
matches2 = {}
matches3 = {}


uppersfkeylookup=set(uppersfkeys)
dfkeylookup=set(dfkeys)
for match in matches:
    matches2.update([(match[1],match[0])])

for index,key in enumerate(sfkeys):
    matches3[uppersfkeys[index]]=key

print("sf to df lookup table")

lut1string=""
lut1=[]

for j,i in enumerate(uppersfkeys):
    
    
  
    if(i in dfkeylookup):
        lut1.append(i)

    else:
        found=False
        match=matches1.get(i)
        if(match!=None):
            found=True
            lut1.append(match)
        

        if(not found):
            lut1.append(dfkeys[0])
    
            
for i in lut1:
    lut1string+="Keyboard::Key::"+i+", "

print(lut1string)

print()
print("df to sf lookup table")

lut2string=""
lut2=[]


for j,i in enumerate(dfkeys):
    
    
    if(i in uppersfkeylookup):
        lut2.append(matches3.get(i))

    else:
        found=False
        match=matches2.get(i)
        if(match!=None):
            found=True
            lut2.append(matches3.get(match))

        if(not found):
            lut2.append(sfkeys[0])
    

for i in lut2:
    lut2string+="sf::Keyboard::Key::"+i+", "

print(lut2string)