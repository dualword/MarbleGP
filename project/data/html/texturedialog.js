      function textureDialogOk() {
        $("#TextureDialog").dialog("close");
        
        var l_Texture = "generate://pattern=" + $("#TexturePattern").val();
        
        var l_Keys = [ "numbercolor", "numberback", "ringcolor", "numberborder", "patterncolor", "patternback" ];

        for (var i = 0; i < l_Keys.length; i++) {
          l_Texture += "&" + l_Keys[i] + "=" + $("#" + l_Keys[i]).val();
        }
        
        g_Profiles[g_EditTextureIndex].texture = l_Texture;
        updateTexture("texture_" + g_EditTextureIndex, g_Profiles[g_EditTextureIndex].texture, g_EditTextureIndex);
        
        g_EditTextureIndex = null;
      }
      
      function textureDialogCancel() {
        $("#TextureDialog").dialog("close");
        
        g_EditTextureIndex = null;
      }
      
      function editTexture(a_Index) {
        g_EditTextureIndex = a_Index;
        
        var l_Texture = getTextureArray(g_Profiles[a_Index].texture);
        
        for (var l_Key in l_Texture) {
          if (l_Key == "pattern") {
            $("#TexturePattern").val(l_Texture[l_Key]);
          }
          else if (l_Key != "type") {
            $("#" + l_Key).val(l_Texture[l_Key]);
            $("#" + l_Key + "_color").val("#" + l_Texture[l_Key]);
          }
        }
        
        $("#TextureDialog").dialog("open");
        $("#TextureDialog").dialog({ title: "Edit Texture of Player \"" + g_Profiles[g_EditTextureIndex].name + "\"." });
      }
      
      function colorchanged(a_This) {
        var l_Id = a_This.id;
        var l_UpdateBorder = false;
        
        if (l_Id == "numberback_color") {
          if ($("#numberback").val() == $("#numberborder").val()) {
            l_UpdateBorder = true;
          }
        }
        
        
        if (l_Id.indexOf("_color") != -1) {
          l_Id = l_Id.substring(0, l_Id.indexOf("_color"));
          $("#" + l_Id).val(a_This.value.substring(1));
          
          if (l_UpdateBorder) {
            $("#numberborder").val(a_This.value.substring(1));
            $("#numberborder_color").val(a_This.value);
          }
        }
      }
