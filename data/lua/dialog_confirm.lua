
g_ConfirmDialogText = nil

function showConfirmDialog(a_Text)
  if g_ConfirmDialogText == nil then
    g_ConfirmDialogText = dialog:getitemfromname("confirmDialog_text")
  end
  
  g_ConfirmDialogText:settext(a_Text)
  
  g_ConfirmDialog = dialog:getitemfromname("confirmDialog")
  g_ConfirmDialog:setvisible(true)
  
  system:setzlayer(99)
end

function confirmDialog_handleButton(a_Id, a_Name)
  local l_Ret = false
  
  if a_Name == "btn_ohisee" then
    if g_ConfirmDialog ~= nil then
      g_ConfirmDialog:setvisible(false)
      g_ConfirmDialog = nil
      l_Ret = true
    end
    system:setzlayer(0)
  end
  
  return l_Ret
end

