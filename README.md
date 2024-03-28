## Giới thiệu dự án

Dự án [cảm biến](https://support.lumi.vn/vi/cam-bien/huongdansudung/Cam-bien-chuyen-dong-V2/zigbee) chuyển động
## thiết bị cho dự án

bộ điều khiển trung tâm của Lumi

cảm biến chuyển động của Lumi

## Tính năng của thiết bị

khi thiết bị được cấp nguồn hoặc thiết bị được reset( nhấn nhanh nút 5 lần) thì thiết bị sẽ tìm mạng nếu có mạng thì thông tin môi trường sẽ được gửi lên HC( bộ điều khiển trung tâm).

khi nhiệt độ thay đổi >= 2 độ thì sẽ cập nhật lên HC.

Khi độ ẩm thay đổi >= 2% thì sẽ cập nhật lên HC.

Khi độ sáng thay đổi >= 50 lux thì sẽ cập nhật lên HC.

Ngoài ra cứ mỗi 30 phút thì thông tin môi trường( nhiệt độ, độ ẩm, ánh sáng) sẽ được cập nhật lên HC định kỳ.

khi có chuyển động thì thiết bị sẽ gửi bản tin phát hiện chuyển động lên HC và trong vòng 30s( thời gian chờ) nếu không có chuyển động nữa thì sẽ gửi bản tin không có chuyển động lên HC.

Ngoài ra ta có thể thay đổi thời gian chờ từ 30s sang 60s hoặc 120s bằng cách đưa thiết bị vào chế độ cài đặt như sau:

1. nhấn giữ nút 10s thiết bị sẽ được đưa vào chế độ cài đặt.

2. chọn thời gian chờ mong muốn bằng cách nhấn nhanh nút: 1 lần để đặt thời gian chờ là 30s, 2 lần để đặt thời gian chờ là 60s, 3 lần là để đặt thời gian chờ là 120s.

## Tài liệu liên quan

[Shematic](https://drive.google.com/file/d/1pZEHfyz2SdoXUbwfalxSU4zjZ-CfnRDe/view?usp=sharing)

[chấn kết nối của vi điều khiển](https://drive.google.com/file/d/1T3HCKvqfHSwt5-0M5fZgJe2qGoGOFFFR/view?usp=sharing)

Vi điều khiển [EFR32MG21](https://www.silabs.com/documents/public/data-sheets/efr32mg21-datasheet.pdf)

cảm biến nhiệt độ độ ấm [Si7020-A20](https://www.mouser.vn/datasheet/2/368/Si7020_A20-1397999.pdf)

cảm biến phát hiện chuyển động [PYD_1598](https://drive.google.com/file/d/1yJATXAYuIUl-gEuYZKAJlrFg8mgOn6yZ/view?usp=sharing)

cảm biến ánh sáng [OPT3001](https://www.ti.com/lit/ds/symlink/opt3001.pdf?ts=1703441103966&ref_url=https%253A%252F%252Fwww.ti.com%252Fproduct%252FOPT3001)

## License

[Minh](https://www.facebook.com/MinhShadows)
