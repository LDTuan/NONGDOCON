var ss = SpreadsheetApp.openById('1TvEjmn0bvlUVXwG1ovH8w_fq7RX5v1fieDlBsXAhCuo');
var name = ss.getSheetByName('Data');

function appendData(giatri1) {
  var date = Utilities.formatDate(new Date(), "GMT+7", "dd/MM/yyyy HH:mm:ss");
  var lastRow = name.getLastRow();
  var stt = lastRow ? lastRow: 1; // Tạo số thứ tự dựa trên hàng cuối cùng
  name.appendRow([stt, date, giatri1]);

  var value1 = parseFloat(giatri1);
  
  // Kiểm tra nếu giá trị vượt ngưỡng
  if (value1 >= 0.25 ) { 
    // Cập nhật giá trị vào cột C 
    sheet.getRange(lastRow + 1, 3).setValue(value1 >= threshold ? value1 : '');
    // Thêm thời gian vào cột bên cạnh (H)
    sheet.getRange(lastRow + 1, 2).setValue(date);
  }

}

function doGet(e) {
  var giatri1 = e.parameter.giatri1;
  appendData(giatri1);
  return ContentService.createTextOutput("Dữ liệu được ghi: " + giatri1 );
}

function doPost(e) {
  var giatri1 = e.parameter.giatri1;
  appendData(giatri2);
  return ContentService.createTextOutput("Dữ liệu được ghi: " + giatri1);
}
