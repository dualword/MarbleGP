      function getColorArray(a_Color) {
        return [ "0x" + a_Color.substring(0, 2), "0x" + a_Color.substring(2, 4), "0x" + a_Color.substring(4, 6), "0xFF" ];
      }
      
      function drawRect(a_Image, a_Rect, a_Color) {
        for (var x = a_Rect.upperleft.x; x < a_Rect.lowerright.x; x++) {
          for (var y = a_Rect.upperleft.y; y < a_Rect.lowerright.y; y++) {
            var l_Index = (x + 512 * y) * 4;
            for (var i = 0; i < 4; i++) {
              a_Image.data[l_Index + i] = a_Color[i];
            }
          }
        }
      }
      
      function drawPattern(a_Image, a_Pattern, a_NewColor, a_Pos) {
        var l_Image  = g_Patterns[a_Pattern];
        
        var l_Canvas = document.createElement("canvas");
        l_Canvas.width  = l_Image.width;
        l_Canvas.height = l_Image.height;
        
        var l_Contxt = l_Canvas.getContext("2d");
        
        l_Contxt.drawImage(l_Image, 0, 0);
        
        var l_Data = l_Contxt.getImageData(0, 0, l_Canvas.width, l_Canvas.height);
        
        for (var y = 0; y < l_Canvas.height; y++) {
          for (var x = 0; x < l_Canvas.width; x++) {
            var l_Target = (x + a_Pos["x"] + 512 * (y + a_Pos["y"])) * 4;
            var l_Index = (x + l_Canvas.width * y) * 4;
            var l_Mix = l_Data.data[l_Index + 3] / 255;
            
            for (var i = 0; i < 3; i++) {
              a_Image.data[l_Target + i] = l_Mix * a_NewColor[i] + (1.0 - l_Mix) * a_Image.data[l_Target + i];
            }
          }
        }
      }
      
      function updateTexture(a_TextureId, a_Texture, a_Player) {
        var l_Canvas = document.getElementById(a_TextureId);
        var l_Contxt = l_Canvas.getContext("2d");
        
        var l_Buffer = l_Contxt.createImageData(512, 512);
        
        var l_ParamMap = getTextureArray(a_Texture);
        
        drawRect(l_Buffer, { "upperleft": { "x": 0, "y":   0 }, "lowerright": { "x": 512, "y": 256 }}, getColorArray(l_ParamMap["numberback" ]));
        drawRect(l_Buffer, { "upperleft": { "x": 0, "y": 256 }, "lowerright": { "x": 512, "y": 512 }}, getColorArray(l_ParamMap["patternback"]));
        
        drawPattern(l_Buffer, "__frame"            , getColorArray(l_ParamMap["ringcolor"   ]), { "x":   0, "y":   0 });
        drawPattern(l_Buffer, l_ParamMap["pattern"], getColorArray(l_ParamMap["patterncolor"]), { "x":   0, "y": 256 });
        drawPattern(l_Buffer, "__numberglow"       , getColorArray(l_ParamMap["numberborder"]), { "x":   0, "y" :  0 });
        drawPattern(l_Buffer, "__numberglow"       , getColorArray(l_ParamMap["numberborder"]), { "x": 256, "y" :  0 });
        drawPattern(l_Buffer, "__number"           , getColorArray(l_ParamMap["numbercolor" ]), { "x":   0, "y" :  0 });
        drawPattern(l_Buffer, "__number"           , getColorArray(l_ParamMap["numbercolor" ]), { "x": 256, "y" :  0 });
        
        l_Contxt.putImageData(l_Buffer, 0, 0);
      }
      
