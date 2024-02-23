      function getColorArray(a_Color) {
        return [ "0x" + a_Color.substring(0, 2), "0x" + a_Color.substring(2, 4), "0x" + a_Color.substring(4, 6), "0xFF" ];
      }
      
      function getTextureArray(a_Texture) {
        var l_Return = { }
        var l_Split = a_Texture.split("://");
        
        l_Return["type"] = l_Split[0];
        
        var l_ParamArray = l_Split[1].split("&");
        
        for (var l_ParamNo = 0; l_ParamNo < l_ParamArray.length; l_ParamNo++) {
          var l_Param = l_ParamArray[l_ParamNo].split("=");
          if (l_Param.length == 2) {
            l_Return[l_Param[0]] = l_Param[1];
          }
        }
        
        return l_Return;
      }
            
      function drawRect(a_Image, a_Rect, a_Color) {
        for (var x = a_Rect.upperleft.x; x < a_Rect.lowerright.x; x++) {
          for (var y = a_Rect.upperleft.y; y < a_Rect.lowerright.y; y++) {
            var l_Index = (x + 256 * y) * 4;
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
            var l_Target = (x + a_Pos["x"] + 256 * (y + a_Pos["y"])) * 4;
            var l_Index = (x + l_Canvas.width * y) * 4;
            var l_Mix = l_Data.data[l_Index + 3] / 255;
            
            for (var i = 0; i < 3; i++) {
              a_Image.data[l_Target + i] = l_Mix * a_NewColor[i] + (1.0 - l_Mix) * a_Image.data[l_Target + i];
            }
          }
        }
      }
      
      function updateTexture(a_TextureId, a_Texture, a_Player) {
        var l_Canvas   = document.getElementById(a_TextureId);
        var l_Contxt   = l_Canvas.getContext("2d");
        var l_Buffer   = l_Contxt.createImageData(256, 256);
        var l_ParamMap = getTextureArray(a_Texture);
        
        if (l_ParamMap["type"] == "generate") {
          if (l_ParamMap["pattern"] == undefined)
            l_ParamMap["pattern"] = "texture_marbles2.png";
          
          drawRect(l_Buffer, { "upperleft": { "x": 0, "y":   0 }, "lowerright": { "x": 256, "y": 128 }}, getColorArray(l_ParamMap["numberback" ]));
          drawRect(l_Buffer, { "upperleft": { "x": 0, "y": 128 }, "lowerright": { "x": 256, "y": 256 }}, getColorArray(l_ParamMap["patternback"]));
          
          drawPattern(l_Buffer, "__frame"            , getColorArray(l_ParamMap["ringcolor"   ]), { "x":   0, "y":   0 });
          drawPattern(l_Buffer, l_ParamMap["pattern"], getColorArray(l_ParamMap["patterncolor"]), { "x":   0, "y": 128 });
          drawPattern(l_Buffer, "__numberglow"       , getColorArray(l_ParamMap["numberborder"]), { "x":   0, "y" :  0 });
          drawPattern(l_Buffer, "__numberglow"       , getColorArray(l_ParamMap["numberborder"]), { "x": 128, "y" :  0 });
          drawPattern(l_Buffer, "__number"           , getColorArray(l_ParamMap["numbercolor" ]), { "x":   0, "y" :  0 });
          drawPattern(l_Buffer, "__number"           , getColorArray(l_ParamMap["numbercolor" ]), { "x": 128, "y" :  0 });
          
          l_Contxt.putImageData(l_Buffer, 0, 0);
        }
        else if (l_ParamMap["type"] == "file") {
          var l_File = l_ParamMap["file"];
          
          if (l_File.startsWith("data/textures/"))
            l_File = l_File.substring("data/textures/".length);
          
          l_Contxt.drawImage(g_Patterns[l_File], 0, 0, 256, 256, 0, 0, 256, 256);
        }
      }
      
