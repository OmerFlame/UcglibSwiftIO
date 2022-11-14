import SwiftIO
import CUcglibSwiftIO
import MadBoard

func makeUnafeMutablePointer<T>(_ p: UnsafeMutablePointer<T>) -> UnsafeMutablePointer<T> {
    return p
}

extension UnsafeMutablePointer {
    mutating func advance(by count: Int) {
        self += count
    }
}

fileprivate var pin_list = [DigitalOut?](repeating: nil, count: Int(UCG_PIN_COUNT))

fileprivate var SPIHandler: SPI!

fileprivate var fontBufsize = 0

public class Ucglib {
    internal var ucg: ucg_t!
    internal var dev_cb: ucg_dev_fnptr!
    internal var ext_cb: ucg_dev_fnptr!
    
    private var tx: ucg_int_t!
    private var ty: ucg_int_t!
    private var tdir: UInt8!
    
    internal func `init`() {
        ucg = ucg_t()
        
        ucg_Init(&ucg, ucg_dev_default_cb, ucg_ext_none, ({_, _, _, _ in return 0}))
        
        tx = 0
        ty = 0
        tdir = 0
        
        for (idx, _) in pin_list.enumerated() {
            pin_list[idx] = nil
        }
    }
    
    public init() {
        `init`()
    }
    
    public init(dev: ucg_dev_fnptr, ext: ucg_dev_fnptr = ucg_ext_none) {
        `init`()
        dev_cb = dev
        ext_cb = ext
    }
    
    public func setPrintPos(x: ucg_int_t, y: ucg_int_t) {
        tx = x
        ty = y
    }
    
    public func getPosX() -> ucg_int_t {
        return tx
    }
    
    public func getPosY() -> ucg_int_t {
        return ty
    }
    
    public func getPrintDir() -> UInt8 {
        return tdir;
    }
    
    public func setPrintDir(dir: UInt8) {
        tdir = dir
    }
    
    @discardableResult
    public func write(_ c: Character) -> Int {
        var delta: ucg_int_t! = nil
        
        delta = ucg_DrawGlyph(getUcg(), getPosX(), getPosY(), getPrintDir(), c.asciiValue!)
        
        switch getPrintDir() {
        case 0:
            tx += delta
            break
            
        case 1:
            ty += delta
            break
            
        case 2:
            tx -= delta
            break
            
        case 3:
            ty -= delta
            break
            
        default:
            break
        }
        
        return 1
    }
    
    public func print(_ s: String) {
        for char in s {
            write(char)
        }
    }
    
    public func getUcg() -> UnsafeMutablePointer<ucg_t>! {
        return makeUnafeMutablePointer(&ucg)
    }
    
    public func getWidth() -> ucg_int_t {
        return ucg_GetWidth(&ucg)
    }
    
    public func getHeight() -> ucg_int_t {
        return ucg_GetHeight(&ucg)
    }
    
    public func setFontRefHeightText() {
        ucg_SetFontRefHeightText(&ucg)
    }
    
    public func setFontRefHeightExtendedText() {
        ucg_SetFontRefHeightExtendedText(&ucg)
    }
    
    public func setFontRefHeightAll() {
        ucg_SetFontRefHeightAll(&ucg)
    }
    
    public func setFontPosBaseline() {
        ucg_SetFontPosBaseline(&ucg)
    }
    
    public func setFontPosBottom() {
        ucg_SetFontPosBottom(&ucg)
    }
    
    public func setFontPosTop() {
        ucg_SetFontPosTop(&ucg)
    }
    
    public func setFontPosCenter() {
        ucg_SetFontPosCenter(&ucg)
    }
    
    public func setFont(_ font: UnsafePointer<ucg_fntpgm_uint8_t>!) {
        ucg_SetFont(&ucg, font)
    }
    
    public func setFont(_ fontName: String) {
        let file = FileDescriptor.open("/SD:/fonts/\(fontName).ufb", .readOnly)
        
		file.seek(offset: 0, from: .end)
		let bufSize = file.tell()
		file.seek(offset: 0, from: .start)
		
        var buf = [UInt8](repeating: 0, count: bufSize)
        
        file.read(into: &buf)
        
        file.close()
        
        var permanentBufPtr: UnsafePointer<UInt8>!
        
        withUnsafePointer(to: buf) { ptr in
            let permanentBuf = UnsafeMutableBufferPointer<UInt8>.allocate(capacity: buf.count)
            
            _ = permanentBuf.initialize(from: buf)
            
            permanentBufPtr = UnsafePointer(permanentBuf.baseAddress)
        }
		
		if ucg.font != nil {
			let mutBufPtr = UnsafeMutableBufferPointer<UInt8>(mutating: UnsafeBufferPointer(start: ucg.font, count: fontBufsize))
			
			print("DEALLOCATING LAST FONT BUFFER!")
			mutBufPtr.deallocate()
		}
        
        ucg_SetFont(&ucg, permanentBufPtr)
		
		fontBufsize = buf.count
    }
    
    public func setFontMode(_ isTransparent: UInt8) {
        ucg_SetFontMode(&ucg, isTransparent)
    }
    
    public func getFontAscent() -> ucg_int_t {
        return ucg_GetFontAscent(&ucg)
    }
    
    public func getFontDescent() -> ucg_int_t {
        return ucg_GetFontDescent(&ucg)
    }
    
    public func getStrWidth(_ s: String) -> ucg_int_t {
        return ucg_GetStrWidth(&ucg, s)
    }
    
    public func setColor(idx: UInt8 = 0, r: UInt8, g: UInt8, b: UInt8) {
        ucg_SetColor(&ucg, idx, r, g, b)
    }
    
    public func undoRotate() {
        ucg_UndoRotate(&ucg)
    }
    
    public func setRotate90() {
        ucg_SetRotate90(&ucg)
    }
    
    public func setRotate180() {
        ucg_SetRotate180(&ucg)
    }
    
    public func setRotate270() {
        ucg_SetRotate270(&ucg)
    }
    
    public func undoScale() {
        ucg_UndoScale(&ucg)
    }
    
    public func setScale2x2() {
        ucg_SetScale2x2(&ucg)
    }
    
    public func powerDown() {
        ucg_PowerDown(&ucg)
    }
    
    public func powerUp() {
        ucg_PowerUp(&ucg)
    }
    
    public func setClipRange(x: ucg_int_t, y: ucg_int_t, w: ucg_int_t, h: ucg_int_t) {
        ucg_SetClipRange(&ucg, x, y, w, h)
    }
    
    public func setMaxClipRange() {
        ucg_SetMaxClipRange(&ucg)
    }
    
    public func drawPixel(x: ucg_int_t, y: ucg_int_t) {
        ucg_DrawPixel(&ucg, x, y)
    }
    
    public func drawHLine(x: ucg_int_t, y: ucg_int_t, len: ucg_int_t) {
        ucg_DrawHLine(&ucg, x, y, len)
    }
    
    public func drawVLine(x: ucg_int_t, y: ucg_int_t, len: ucg_int_t) {
        ucg_DrawVLine(&ucg, x, y, len)
    }
    
    public func drawLine(x1: ucg_int_t, y1: ucg_int_t, x2: ucg_int_t, y2: ucg_int_t) {
        ucg_DrawLine(&ucg, x1, y1, x2, y2)
    }
    
    public func drawGlyph(x: ucg_int_t, y: ucg_int_t, dir: UInt8, encoding: UInt8) -> ucg_int_t {
        return ucg_DrawGlyph(&ucg, x, y, dir, encoding)
    }
    
    public func drawString(x: ucg_int_t, y: ucg_int_t, dir: UInt8, str: String) -> ucg_int_t {
        return ucg_DrawString(&ucg, x, y, dir, str)
    }
    
    public func drawBox(x: ucg_int_t, y: ucg_int_t, w: ucg_int_t, h: ucg_int_t) {
        ucg_DrawBox(&ucg, x, y, w, h)
    }
    
    public func clearScreen() {
        ucg_ClearScreen(&ucg)
    }
    
    public func drawRBox(x: ucg_int_t, y: ucg_int_t, w: ucg_int_t, h: ucg_int_t, r: ucg_int_t) {
        ucg_DrawRBox(&ucg, x, y, w, h, r)
    }
    
    public func drawFrame(x: ucg_int_t, y: ucg_int_t, w: ucg_int_t, h: ucg_int_t) {
        ucg_DrawFrame(&ucg, x, y, w, h)
    }
    
    public func drawRFrame(x: ucg_int_t, y: ucg_int_t, w: ucg_int_t, h: ucg_int_t, r: ucg_int_t) {
        ucg_DrawRFrame(&ucg, x, y, w, h, r)
    }
    
    public func drawDisc(x0: ucg_int_t, y0: ucg_int_t, rad: ucg_int_t, option: UInt8) {
        ucg_DrawDisc(&ucg, x0, y0, rad, option)
    }
    
    public func drawCircle(x0: ucg_int_t, y0: ucg_int_t, rad: ucg_int_t, option: UInt8) {
        ucg_DrawCircle(&ucg, x0, y0, rad, option)
    }
    
    public func drawTriangle(x0: Int16, y0: Int16, x1: Int16, y1: Int16, x2: Int16, y2: Int16) {
        ucg_DrawTriangle(&ucg, x0, y0, x1, y1, x2, y2)
    }
    
    public func drawTetragon(x0: Int16, y0: Int16, x1: Int16, y1: Int16, x2: Int16, y2: Int16, x3: Int16, y3: Int16) {
        ucg_DrawTetragon(&ucg, x0, y0, x1, y1, x2, y2, x3, y3)
    }
    
    public func drawGradientLine(x: ucg_int_t, y: ucg_int_t, len: ucg_int_t, dir: ucg_int_t) {
        ucg_DrawGradientLine(&ucg, x, y, len, dir)
    }
    
    public func drawGradientBox(x: ucg_int_t, y: ucg_int_t, w: ucg_int_t, h: ucg_int_t) {
        ucg_DrawGradientBox(&ucg, x, y, w, h)
    }
}

public class Ucglib4WireSWSPI: Ucglib {
    public init(dev: ucg_dev_fnptr, ext: ucg_dev_fnptr, scl: IdName, sda: IdName, cd: IdName, cs: IdName?, reset: IdName?) {
        super.init()
        
        dev_cb = dev
        ext_cb = ext
        
        pin_list[Int(UCG_PIN_SCL)] = DigitalOut(scl)
        pin_list[Int(UCG_PIN_SDA)] = DigitalOut(sda)
        
        pin_list[Int(UCG_PIN_CD)] = DigitalOut(cd)
        
        pin_list[Int(UCG_PIN_RST)] = reset != nil ? DigitalOut(reset!) : nil
        pin_list[Int(UCG_PIN_CS)] = cs != nil ? DigitalOut(cs!) : nil
    }
    
    public func begin(isTransparent: UInt8) {
        ucg_Init(&ucg, dev_cb, ext_cb, ucg_com_arduino_generic_SW_SPI)
        ucg_SetFontMode(&ucg, isTransparent)
    }
}

public class Ucglib3WireILI9325SWSPI: Ucglib {
    public init(dev: ucg_dev_fnptr, ext: ucg_dev_fnptr, scl: IdName, sda: IdName, cs: IdName?, reset: IdName?) {
        super.init()
        
        dev_cb = dev
        ext_cb = ext
        
        pin_list[Int(UCG_PIN_SCL)] = DigitalOut(scl)
        pin_list[Int(UCG_PIN_SDA)] = DigitalOut(sda)
        
        pin_list[Int(UCG_PIN_RST)] = reset != nil ? DigitalOut(reset!) : nil
        pin_list[Int(UCG_PIN_CS)] = cs != nil ? DigitalOut(cs!) : nil
    }
    
    public func begin(isTransparent: UInt8) {
        ucg_Init(&ucg, dev_cb, ext_cb, ucg_com_arduino_ili9325_SW_SPI)
        ucg_SetFontMode(&ucg, isTransparent)
    }
}

public class Ucglib3Wire9bitSWSPI: Ucglib {
    public init(dev: ucg_dev_fnptr, ext: ucg_dev_fnptr, scl: IdName, sda: IdName, cs: IdName?, reset: IdName?) {
        super.init()
        
        dev_cb = dev
        ext_cb = ext
        
        pin_list[Int(UCG_PIN_SCL)] = DigitalOut(scl)
        pin_list[Int(UCG_PIN_SDA)] = DigitalOut(sda)
        
        pin_list[Int(UCG_PIN_RST)] = reset != nil ? DigitalOut(reset!) : nil
        pin_list[Int(UCG_PIN_CS)] = cs != nil ? DigitalOut(cs!) : nil
    }
    
    public func begin(isTransparent: UInt8) {
        ucg_Init(&ucg, dev_cb, ext_cb, ucg_com_arduino_3wire_9bit_SW_SPI);
        ucg_SetFontMode(&ucg, isTransparent);
    }
}

public class Ucglib4WireHWSPI: Ucglib {
    public init(spi: SPI, dev: ucg_dev_fnptr, ext: ucg_dev_fnptr, cd: IdName, cs: IdName?, reset: IdName?) {
        super.init()
        
        dev_cb = dev
        ext_cb = ext
        
        pin_list[Int(UCG_PIN_RST)] = reset != nil ? DigitalOut(reset!) : nil
        pin_list[Int(UCG_PIN_CS)] = cs != nil ? DigitalOut(cs!) : nil
        pin_list[Int(UCG_PIN_CD)] = DigitalOut(cd)
        
        SPIHandler = spi
    }
    
    public func begin(isTransparent: UInt8) {
        ucg_Init(&ucg, dev_cb, ext_cb, ucg_com_arduino_4wire_HW_SPI)
        ucg_SetFontMode(&ucg, isTransparent)
    }
}

public class Ucglib3Wire9bitHWSPI: Ucglib {
    public init(spi: SPI, dev: ucg_dev_fnptr, ext: ucg_dev_fnptr, cs: IdName?, reset: IdName?) {
        super.init()
        
        dev_cb = dev
        ext_cb = ext
        
        pin_list[Int(UCG_PIN_RST)] = reset != nil ? DigitalOut(reset!) : nil
        pin_list[Int(UCG_PIN_CS)] = cs != nil ? DigitalOut(cs!) : nil
    }
    
    public func begin(isTransparent: UInt8) {
        ucg_Init(&ucg, dev_cb, ext_cb, ucg_com_arduino_3wire_9bit_HW_SPI)
        ucg_SetFontMode(&ucg, isTransparent)
    }
}

public class Ucglib8Bit: Ucglib {
    public init(dev: ucg_dev_fnptr, ext: ucg_dev_fnptr, d0: IdName, d1: IdName, d2: IdName, d3: IdName, d4: IdName, d5: IdName, d6: IdName?, d7: IdName?, wr: IdName, cd: IdName, cs: IdName?, reset: IdName?) {
        super.init()
        
        dev_cb = dev
        ext_cb = ext
        
        pin_list[Int(UCG_PIN_RST)] = reset != nil ? DigitalOut(reset!) : nil
        pin_list[Int(UCG_PIN_CS)] = cs != nil ? DigitalOut(cs!) : nil
        pin_list[Int(UCG_PIN_CD)] = DigitalOut(cd)
        
        pin_list[Int(UCG_PIN_WR)] = DigitalOut(wr)
        pin_list[Int(UCG_PIN_D0)] = DigitalOut(d0)
        pin_list[Int(UCG_PIN_D1)] = DigitalOut(d1)
        pin_list[Int(UCG_PIN_D2)] = DigitalOut(d2)
        pin_list[Int(UCG_PIN_D3)] = DigitalOut(d3)
        pin_list[Int(UCG_PIN_D4)] = DigitalOut(d4)
        pin_list[Int(UCG_PIN_D5)] = DigitalOut(d5)
        pin_list[Int(UCG_PIN_D6)] = d6 != nil ? DigitalOut(d6!) : nil
        pin_list[Int(UCG_PIN_D7)] = d7 != nil ? DigitalOut(d7!) : nil
    }
    
    public func begin(isTransparent: UInt8) {
        ucg_Init(&ucg, dev_cb, ext_cb, ucg_com_arduino_generic_8bit)
        ucg_SetFontMode(&ucg, isTransparent)
    }
}

public class Ucglib_ST7735_18x128x160_HWSPI: Ucglib4WireHWSPI {
    public init(spi: SPI, cd: IdName, cs: IdName?, reset: IdName?) {
        super.init(spi: spi, dev: ucg_dev_st7735_18x128x160, ext: ucg_ext_st7735_18, cd: cd, cs: cs, reset: reset)
    }
}

public class Ucglib_ST7735_18x128x160_SWSPI: Ucglib4WireSWSPI {
    public init(scl: IdName, sda: IdName, cd: IdName, cs: IdName?, reset: IdName?) {
        super.init(dev: ucg_dev_st7735_18x128x160, ext: ucg_ext_st7735_18, scl: scl, sda: sda, cd: cd, cs: cs, reset: reset)
    }
}

public class Ucglib_ILI9341_18x240x320_HWSPI: Ucglib4WireHWSPI {
    public init(spi: SPI, cd: IdName, cs: IdName?, reset: IdName?) {
        super.init(spi: spi, dev: ucg_dev_ili9341_18x240x320, ext: ucg_ext_ili9341_18, cd: cd, cs: cs, reset: reset)
    }
}

public class Ucglib_ILI9341_18x240x320_SWSPI: Ucglib4WireSWSPI {
    public init(scl: IdName, sda: IdName, cd: IdName, cs: IdName?, reset: IdName?) {
        super.init(dev: ucg_dev_ili9341_18x240x320, ext: ucg_ext_ili9341_18, scl: scl, sda: sda, cd: cd, cs: cs, reset: reset)
    }
}

public class Ucglib_HX8352C_18x240x400_HWSPI: Ucglib4WireHWSPI {
    public init(spi: SPI, cd: IdName, cs: IdName?, reset: IdName?) {
        super.init(spi: spi, dev: ucg_dev_hx8352c_18x240x400, ext: ucg_ext_hx8352c_18, cd: cd, cs: cs, reset: reset)
    }
}

public class Ucglib_HX8352C_18x240x400_SWSPI: Ucglib4WireSWSPI {
    public init(scl: IdName, sda: IdName, cd: IdName, cs: IdName?, reset: IdName?) {
        super.init(dev: ucg_dev_hx8352c_18x240x400, ext: ucg_ext_hx8352c_18, scl: scl, sda: sda, cd: cd, cs: cs, reset: reset)
    }
}

public class Ucglib_ILI9486_18x320x480_HWSPI: Ucglib4WireHWSPI {
    public init(spi: SPI, cd: IdName, cs: IdName?, reset: IdName?) {
        super.init(spi: spi, dev: ucg_dev_ili9486_18x320x480, ext: ucg_ext_ili9486_18, cd: cd, cs: cs, reset: reset)
    }
}

public class Ucglib_ILI9486_18x320x480_SWSPI: Ucglib4WireSWSPI {
    public init(scl: IdName, sda: IdName, cd: IdName, cs: IdName?, reset: IdName?) {
        super.init(dev: ucg_dev_ili9486_18x320x480, ext: ucg_ext_ili9486_18, scl: scl, sda: sda, cd: cd, cs: cs, reset: reset)
    }
}

public class Ucglib_ILI9163_18x128x128_HWSPI: Ucglib4WireHWSPI {
    public init(spi: SPI, cd: IdName, cs: IdName?, reset: IdName?) {
        super.init(spi: spi, dev: ucg_dev_ili9163_18x128x128, ext: ucg_ext_ili9163_18, cd: cd, cs: cs, reset: reset)
    }
}

public class Ucglib_ILI9163_18x128x128_SWSPI: Ucglib4WireSWSPI {
    public init(scl: IdName, sda: IdName, cd: IdName, cs: IdName?, reset: IdName?) {
        super.init(dev: ucg_dev_ili9163_18x128x128, ext: ucg_ext_ili9163_18, scl: scl, sda: sda, cd: cd, cs: cs, reset: reset)
    }
}

public class Ucglib_SSD1351_18x128x128_HWSPI: Ucglib4WireHWSPI {
    public init(spi: SPI, cd: IdName, cs: IdName?, reset: IdName?) {
        super.init(spi: spi, dev: ucg_dev_ssd1351_18x128x128_ilsoft, ext: ucg_ext_ssd1351_18, cd: cd, cs: cs, reset: reset)
    }
}

public class Ucglib_SSD1351_18x128x128_SWSPI: Ucglib4WireSWSPI {
    public init(scl: IdName, sda: IdName, cd: IdName, cs: IdName?, reset: IdName?) {
        super.init(dev: ucg_dev_ssd1351_18x128x128_ilsoft, ext: ucg_ext_ssd1351_18, scl: scl, sda: sda, cd: cd, cs: cs, reset: reset)
    }
}

public class Ucglib_SSD1351_18x128x128_FT_HWSPI: Ucglib4WireHWSPI {
    public init(spi: SPI, cd: IdName, cs: IdName?, reset: IdName?) {
        super.init(spi: spi, dev: ucg_dev_ssd1351_18x128x128_ft, ext: ucg_ext_ssd1351_18, cd: cd, cs: cs, reset: reset)
    }
}

public class Ucglib_SSD1351_18x128x128_FT_SWSPI: Ucglib4WireSWSPI {
    public init(scl: IdName, sda: IdName, cd: IdName, cs: IdName?, reset: IdName?) {
        super.init(dev: ucg_dev_ssd1351_18x128x128_ft, ext: ucg_ext_ssd1351_18, scl: scl, sda: sda, cd: cd, cs: cs, reset: reset)
    }
}

public class Ucglib_PCF8833_16x132x132_HWSPI: Ucglib3Wire9bitHWSPI {
    public init(spi: SPI, cs: IdName?, reset: IdName?) {
        super.init(spi: spi, dev: ucg_dev_pcf8833_16x132x132, ext: ucg_ext_pcf8833_16, cs: cs, reset: reset)
    }
}

public class Ucglib_PCF8833_16x132x132_SWSPI: Ucglib3Wire9bitSWSPI {
    public init(scl: IdName, sda: IdName, cd: IdName, cs: IdName?, reset: IdName?) {
        super.init(dev: ucg_dev_pcf8833_16x132x132, ext: ucg_ext_pcf8833_16, scl: scl, sda: sda, cs: cs, reset: reset)
    }
}

public class Ucglib_LD50T6160_18x160x128_6Bit: Ucglib8Bit {
    public init(d0: IdName, d1: IdName, d2: IdName, d3: IdName, d4: IdName, d5: IdName, wr: IdName, cd: IdName, cs: IdName?, reset: IdName?) {
        super.init(dev: ucg_dev_ld50t6160_18x160x128_samsung, ext: ucg_ext_ld50t6160_18, d0: d0, d1: d1, d2: d2, d3: d3, d4: d4, d5: d5, d6: nil, d7: nil, wr: wr, cd: cd, cs: cs, reset: reset)
    }
}

public class Ucglib_SSD1331_18x96x64_UNIVISION_HWSPI: Ucglib4WireHWSPI {
    public init(spi: SPI, cd: IdName, cs: IdName?, reset: IdName?) {
        super.init(spi: spi, dev: ucg_dev_ssd1331_18x96x64_univision, ext: ucg_ext_ssd1331_18, cd: cd, cs: cs, reset: reset)
    }
}

public class Ucglib_SSD1331_18x96x64_UNIVISION_SWSPI: Ucglib4WireSWSPI {
    public init(scl: IdName, sda: IdName, cd: IdName, cs: IdName?, reset: IdName?) {
        super.init(dev: ucg_dev_ssd1331_18x96x64_univision, ext: ucg_ext_ssd1331_18, scl: scl, sda: sda, cd: cd, cs: cs, reset: reset)
    }
}

public class Ucglib_SEPS225_16x128x128_UNIVISION_HWSPI: Ucglib4WireHWSPI {
    public init(spi: SPI, cd: IdName, cs: IdName?, reset: IdName?) {
        super.init(spi: spi, dev: ucg_dev_seps225_16x128x128_univision, ext: ucg_ext_seps225_16, cd: cd, cs: cs, reset: reset)
    }
}

public class Ucglib_SEPS225_16x128x128_UNIVISION_SWSPI: Ucglib4WireSWSPI {
    public init(scl: IdName, sda: IdName, cd: IdName, cs: IdName?, reset: IdName?) {
        super.init(dev: ucg_dev_seps225_16x128x128_univision, ext: ucg_ext_seps225_16, scl: scl, sda: sda, cd: cd, cs: cs, reset: reset)
    }
}

// MARK: - 8-bit SW SPI

func ucg_com_arduino_send_generic_SW_SPI(ucg: UnsafeMutablePointer<ucg_t>!, data: UInt8) {
    var data = data
    var i: UInt8 = 8
    
    var sclPin = pin_list[Int(UCG_PIN_SCL)]!
    var sdaPin = pin_list[Int(UCG_PIN_SDA)]!
    
    repeat {
        if (data & 128) != 0 {
            sdaPin.high()
        } else {
            sdaPin.low()
        }
        
        wait(us: 1)
        sclPin.high()
        wait(us: 1)
        i -= 1
        sclPin.low()
        wait(us: 1)
        data <<= 1
    } while i > 0
}

func ucg_com_arduino_generic_SW_SPI(ucg: UnsafeMutablePointer<ucg_t>!, msg: Int16, arg: UInt16, data: UnsafeMutablePointer<UInt8>!) -> Int16 {
    switch Int32(msg) {
    case UCG_COM_MSG_POWER_UP:
        let cdPin = pin_list[Int(UCG_PIN_CD)]!
        let sdaPin = pin_list[Int(UCG_PIN_SDA)]!
        let sclPin = pin_list[Int(UCG_PIN_SCL)]!
        
        let csPin = pin_list[Int(UCG_PIN_CS)]
        let rstPin = pin_list[Int(UCG_PIN_RST)]
        
        cdPin.high()
        sdaPin.high()
        sclPin.low()
        csPin?.high()
        rstPin?.high()
        break
        
    case UCG_COM_MSG_POWER_DOWN:
        break
        
    case UCG_COM_MSG_DELAY:
        wait(us: Int(arg))
        break
        
    case UCG_COM_MSG_CHANGE_RESET_LINE:
        let rstPin = pin_list[Int(UCG_PIN_RST)]
        
        rstPin?.write(arg != 0 ? true : false)
        break
        
    case UCG_COM_MSG_CHANGE_CS_LINE:
        let csPin = pin_list[Int(UCG_PIN_CS)]
        
        csPin?.write(arg != 0 ? true : false)
        break
        
    case UCG_COM_MSG_CHANGE_CD_LINE:
        let cdPin = pin_list[Int(UCG_PIN_CD)]!
        
        cdPin.write(arg != 0 ? true : false)
        break
        
    case UCG_COM_MSG_SEND_BYTE:
        ucg_com_arduino_send_generic_SW_SPI(ucg: ucg, data: UInt8(arg))
        break
        
    case UCG_COM_MSG_REPEAT_1_BYTE:
        var arg = arg
        
        while arg > 0 {
            ucg_com_arduino_send_generic_SW_SPI(ucg: ucg, data: data.pointee)
            arg -= 1
        }
        break
        
    case UCG_COM_MSG_REPEAT_2_BYTES:
        var arg = arg
        
        while arg > 0 {
            ucg_com_arduino_send_generic_SW_SPI(ucg: ucg, data: data.pointee)
            ucg_com_arduino_send_generic_SW_SPI(ucg: ucg, data: data.advanced(by: 1).pointee)
            arg -= 1
        }
        break
        
    case UCG_COM_MSG_REPEAT_3_BYTES:
        var arg = arg
        
        while arg > 0 {
            ucg_com_arduino_send_generic_SW_SPI(ucg: ucg, data: data.pointee)
            ucg_com_arduino_send_generic_SW_SPI(ucg: ucg, data: data.advanced(by: 1).pointee)
            ucg_com_arduino_send_generic_SW_SPI(ucg: ucg, data: data.advanced(by: 2).pointee)
            arg -= 1
        }
        break
        
    case UCG_COM_MSG_SEND_STR:
        var arg = arg
        var data = data!
        
        while arg > 0 {
            ucg_com_arduino_send_generic_SW_SPI(ucg: ucg, data: data.pointee)
            
            data = data.advanced(by: 1)
            arg -= 1
        }
        break
        
    case UCG_COM_MSG_SEND_CD_DATA_SEQUENCE:
        var arg = arg
        var data = data!
        
        let cdPin = pin_list[Int(UCG_PIN_CD)]!
        
        while arg > 0 {
            if data.pointee != 0 {
                if data.pointee == 1 {
                    cdPin.low()
                } else {
                    cdPin.high()
                }
            }
            
            data = data.advanced(by: 1)
            ucg_com_arduino_send_generic_SW_SPI(ucg: ucg, data: data.pointee)
            data = data.advanced(by: 1)
            arg -= 1
        }
        break
        
    default:
        break
    }
    
    return 1
}

// MARK: - 8-bit SW SPI for ILI9325 (mode IM3=0, IM2=1, IM1=0, IM0=0)

func ucg_com_arduino_ili9325_SW_SPI(ucg: UnsafeMutablePointer<ucg_t>!, msg: Int16, arg: UInt16, data: UnsafeMutablePointer<UInt8>!) -> Int16 {
    switch Int32(msg) {
    case UCG_COM_MSG_POWER_UP:
        pin_list[Int(UCG_PIN_CD)]!.write(true)
        pin_list[Int(UCG_PIN_SDA)]!.write(true)
        pin_list[Int(UCG_PIN_SCL)]!.write(false)
        pin_list[Int(UCG_PIN_CS)]?.write(true)
        pin_list[Int(UCG_PIN_RST)]?.write(true)
        break
        
    case UCG_COM_MSG_POWER_DOWN:
        break
        
    case UCG_COM_MSG_DELAY:
        wait(us: Int(arg))
        break
        
    case UCG_COM_MSG_CHANGE_RESET_LINE:
        pin_list[Int(UCG_PIN_RST)]?.write(arg != 0 ? true : false)
        break
        
    case UCG_COM_MSG_CHANGE_CS_LINE:
        pin_list[Int(UCG_PIN_CS)]?.write(arg != 0 ? true : false)
        break
        
    case UCG_COM_MSG_CHANGE_CD_LINE:
        pin_list[Int(UCG_PIN_CS)]?.write(true)
        pin_list[Int(UCG_PIN_CS)]?.write(false)
        
        if (ucg.pointee.com_status & UInt8(UCG_COM_STATUS_MASK_CD)) != 0 {
            ucg_com_arduino_send_generic_SW_SPI(ucg: ucg, data: 0x072)
        } else {
            ucg_com_arduino_send_generic_SW_SPI(ucg: ucg, data: 0x070)
        }
        break
        
    case UCG_COM_MSG_SEND_BYTE:
        ucg_com_arduino_send_generic_SW_SPI(ucg: ucg, data: UInt8(arg))
        break
        
    case UCG_COM_MSG_REPEAT_1_BYTE:
        var arg = arg
        
        while arg > 0 {
            ucg_com_arduino_send_generic_SW_SPI(ucg: ucg, data: data.pointee)
            arg -= 1
        }
        break
        
    case UCG_COM_MSG_REPEAT_2_BYTES:
        var arg = arg
        
        while arg > 0 {
            ucg_com_arduino_send_generic_SW_SPI(ucg: ucg, data: data.pointee)
            ucg_com_arduino_send_generic_SW_SPI(ucg: ucg, data: data.advanced(by: 1).pointee)
            arg -= 1
        }
        break
        
    case UCG_COM_MSG_REPEAT_3_BYTES:
        var arg = arg
        
        while arg > 0 {
            ucg_com_arduino_send_generic_SW_SPI(ucg: ucg, data: data.pointee)
            ucg_com_arduino_send_generic_SW_SPI(ucg: ucg, data: data.advanced(by: 1).pointee)
            ucg_com_arduino_send_generic_SW_SPI(ucg: ucg, data: data.advanced(by: 2).pointee)
            arg -= 1
        }
        break
        
    case UCG_COM_MSG_SEND_STR:
        var arg = arg
        var data = data!
        
        while arg > 0 {
            ucg_com_arduino_send_generic_SW_SPI(ucg: ucg, data: data.pointee)
            
            arg -= 1
            data = data.advanced(by: 1)
        }
        break
        
    case UCG_COM_MSG_SEND_CD_DATA_SEQUENCE:
        var arg = arg
        var data = data!
        var csPin = pin_list[Int(UCG_PIN_CS)]
        
        while arg > 0 {
            if data.pointee != 0 {
                if data.pointee == 1 {
                    csPin?.write(true)
                    csPin?.write(false)
                    
                    ucg_com_arduino_send_generic_SW_SPI(ucg: ucg, data: 0x070)
                } else {
                    csPin?.write(true)
                    csPin?.write(false)
                    
                    ucg_com_arduino_send_generic_SW_SPI(ucg: ucg, data: 0x072)
                }
            }
            
            data = data.advanced(by: 1)
            ucg_com_arduino_send_generic_SW_SPI(ucg: ucg, data: data.pointee)
            data = data.advanced(by: 1)
            arg -= 1
        }
        break
        
    default:
        break
    }
    
    return 1
}

// MARK: - 9-bit SW SPI

func ucg_com_arduino_send_3wire_9bit_SW_SPI(ucg: UnsafeMutablePointer<ucg_t>!, firstBit: UInt8, data: UInt8) {
    let sdaPin = pin_list[Int(UCG_PIN_SDA)]!
    let sclPin = pin_list[Int(UCG_PIN_SCL)]!
    
    var i: UInt8! = 8
    var data = data
    
    if firstBit != 0 {
        sdaPin.write(true)
    } else {
        sdaPin.write(false)
    }
    
    sclPin.write(true)
    sclPin.write(false)
    
    repeat {
        if (data & 128) != 0 {
            sdaPin.write(true)
        } else {
            sdaPin.write(false)
        }
        
        sclPin.write(true)
        
        i -= 1
        
        sclPin.write(false)
        
        data <<= 1
    } while i > 0
}

public func ucg_com_arduino_3wire_9bit_SW_SPI(ucg: UnsafeMutablePointer<ucg_t>!, msg: Int16, arg: UInt16, data: UnsafeMutablePointer<UInt8>!) -> Int16 {
    switch Int32(msg) {
    case UCG_COM_MSG_POWER_UP:
        pin_list[Int(UCG_PIN_SDA)]!.write(true)
        pin_list[Int(UCG_PIN_SCL)]!.write(false)
        pin_list[Int(UCG_PIN_CS)]?.write(true)
        pin_list[Int(UCG_PIN_RST)]?.write(true)
        break
        
    case UCG_COM_MSG_POWER_DOWN:
        break
        
    case UCG_COM_MSG_DELAY:
        wait(us: Int(arg))
        break
        
    case UCG_COM_MSG_CHANGE_RESET_LINE:
        pin_list[Int(UCG_PIN_RST)]?.write(arg != 0 ? true : false)
        break
        
    case UCG_COM_MSG_CHANGE_CS_LINE:
        pin_list[Int(UCG_PIN_CS)]?.write(arg != 0 ? true : false)
        break
        
    case UCG_COM_MSG_CHANGE_CD_LINE:
        // Ignored since there is no CD line in this configuration
        break
        
    case UCG_COM_MSG_SEND_BYTE:
        ucg_com_arduino_send_3wire_9bit_SW_SPI(ucg: ucg, firstBit: ucg.pointee.com_status & UInt8(UCG_COM_STATUS_MASK_CD), data: data.pointee)
        break
        
    case UCG_COM_MSG_REPEAT_1_BYTE:
        var arg = arg
        
        while arg > 0 {
            ucg_com_arduino_send_3wire_9bit_SW_SPI(ucg: ucg, firstBit: ucg.pointee.com_status & UInt8(UCG_COM_STATUS_MASK_CD), data: data.pointee)
            arg -= 1
        }
        break
        
    case UCG_COM_MSG_REPEAT_2_BYTES:
        var arg = arg
        
        while arg > 0 {
            ucg_com_arduino_send_3wire_9bit_SW_SPI(ucg: ucg, firstBit: ucg.pointee.com_status & UInt8(UCG_COM_STATUS_MASK_CD), data: data.pointee)
            ucg_com_arduino_send_3wire_9bit_SW_SPI(ucg: ucg, firstBit: ucg.pointee.com_status & UInt8(UCG_COM_STATUS_MASK_CD), data: data.advanced(by: 1).pointee)
            arg -= 1
        }
        break
        
    case UCG_COM_MSG_REPEAT_3_BYTES:
        var arg = arg
        
        while arg > 0 {
            ucg_com_arduino_send_3wire_9bit_SW_SPI(ucg: ucg, firstBit: ucg.pointee.com_status & UInt8(UCG_COM_STATUS_MASK_CD), data: data.pointee)
            ucg_com_arduino_send_3wire_9bit_SW_SPI(ucg: ucg, firstBit: ucg.pointee.com_status & UInt8(UCG_COM_STATUS_MASK_CD), data: data.advanced(by: 1).pointee)
            ucg_com_arduino_send_3wire_9bit_SW_SPI(ucg: ucg, firstBit: ucg.pointee.com_status & UInt8(UCG_COM_STATUS_MASK_CD), data: data.advanced(by: 2).pointee)
            arg -= 1
        }
        break
        
    case UCG_COM_MSG_SEND_STR:
        var arg = arg
        var data = data!
        
        while arg > 0 {
            ucg_com_arduino_send_3wire_9bit_SW_SPI(ucg: ucg, firstBit: ucg.pointee.com_status & UInt8(UCG_COM_STATUS_MASK_CD), data: data.pointee)
            
            arg -= 1
            data = data.advanced(by: 1)
        }
        break
        
    case UCG_COM_MSG_SEND_CD_DATA_SEQUENCE:
        var lastCD = ucg.pointee.com_status & UInt8(UCG_COM_STATUS_MASK_CD)
        var arg = arg
        var data = data!
        
        while arg > 0 {
            if data.pointee != 0 {
                if data.pointee == 1 {
                    lastCD = 0
                } else {
                    lastCD = 1
                }
            }
            
            data = data.advanced(by: 1)
            ucg_com_arduino_send_3wire_9bit_SW_SPI(ucg: ucg, firstBit: lastCD, data: data.pointee)
            data = data.advanced(by: 1)
            arg -= 1
        }
        break
        
    default:
        break
    }
    
    return 1
}

// MARK: - 9-bit HW SPI

let UCG_COM_ARDUINO_3WIRE_8BIT_BUF_LEN = 9
var ucg_com_3wire_9bit_buffer = [UInt8](repeating: 0, count: UCG_COM_ARDUINO_3WIRE_8BIT_BUF_LEN)
var ucg_com_3wire_9bit_buf_bytepos: UInt8 = 0
var ucg_com_3wire_9bit_buf_bitpos: UInt8 = 0
var ucg_com_3wire_9bit_cd_mask: UInt8 = 0

func ucg_com_arduino_init_3wire_9bit_HW_SPI(ucg: UnsafeMutablePointer<ucg_t>!) {
    ucg_com_3wire_9bit_buf_bytepos = 0
    ucg_com_3wire_9bit_buf_bitpos = 7
    ucg_com_3wire_9bit_cd_mask = 128
    
    for (i, _) in ucg_com_3wire_9bit_buffer.enumerated() {
        ucg_com_3wire_9bit_buffer[i] = 0
    }
}

func ucg_com_arduino_flush_3wire_9bit_HW_SPI(ucg: UnsafeMutablePointer<ucg_t>!) {
    if ucg_com_3wire_9bit_buf_bytepos == 0 && ucg_com_3wire_9bit_buf_bitpos == 7 {
        return
    }
    
    for (i, _) in ucg_com_3wire_9bit_buffer.enumerated() {
        SPIHandler.write(ucg_com_3wire_9bit_buffer[i])
    }
    
    ucg_com_arduino_init_3wire_9bit_HW_SPI(ucg: ucg)
}

func ucg_com_arduino_send_3wire_9bit_HW_SPI(ucg: UnsafeMutablePointer<ucg_t>!, firstBit: UInt8, data: UInt8) {
    if firstBit != 0 {
        ucg_com_3wire_9bit_buffer[Int(ucg_com_3wire_9bit_buf_bytepos)] |= ucg_com_3wire_9bit_cd_mask
    }
    
    if ucg_com_3wire_9bit_buf_bitpos > 0 {
        ucg_com_3wire_9bit_buf_bitpos -= 1
        ucg_com_3wire_9bit_cd_mask >>= 1
    } else {
        ucg_com_3wire_9bit_buf_bitpos = 7
        ucg_com_3wire_9bit_buf_bytepos += 1
        ucg_com_3wire_9bit_cd_mask = 128
    }
    
    ucg_com_3wire_9bit_buffer[Int(ucg_com_3wire_9bit_buf_bytepos)] |= data >> (7 - ucg_com_3wire_9bit_buf_bitpos)
    
    if ucg_com_3wire_9bit_buf_bitpos == 7 {
        ucg_com_3wire_9bit_buf_bytepos += 1
        
        if ucg_com_3wire_9bit_buf_bytepos >= UCG_COM_ARDUINO_3WIRE_8BIT_BUF_LEN {
            ucg_com_arduino_flush_3wire_9bit_HW_SPI(ucg: ucg)
        }
    } else {
        ucg_com_3wire_9bit_buf_bytepos += 1
        ucg_com_3wire_9bit_buffer[Int(ucg_com_3wire_9bit_buf_bytepos)] |= data << (ucg_com_3wire_9bit_buf_bitpos + 1)
    }
}

func ucg_com_arduino_3wire_9bit_HW_SPI(ucg: UnsafeMutablePointer<ucg_t>!, msg: Int16, arg: UInt16, data: UnsafeMutablePointer<UInt8>!) -> Int16 {
    switch Int32(msg) {
    case UCG_COM_MSG_POWER_UP:
        ucg_com_arduino_init_3wire_9bit_HW_SPI(ucg: ucg)
        
        pin_list[Int(UCG_PIN_CS)]?.write(true)
        pin_list[Int(UCG_PIN_RST)]?.write(true)
        break
        
    case UCG_COM_MSG_POWER_DOWN:
        break
        
    case UCG_COM_MSG_DELAY:
        ucg_com_arduino_flush_3wire_9bit_HW_SPI(ucg: ucg)
        wait(us: Int(arg))
        break
        
    case UCG_COM_MSG_CHANGE_RESET_LINE:
        pin_list[Int(UCG_PIN_RST)]?.write(arg != 0 ? true : false)
        break
        
    case UCG_COM_MSG_CHANGE_CS_LINE:
        if arg != 0 {
            ucg_com_arduino_flush_3wire_9bit_HW_SPI(ucg: ucg)
        }
        
        pin_list[Int(UCG_PIN_CS)]?.write(arg != 0 ? true : false)
        
        if arg != 0 {
            ucg_com_arduino_init_3wire_9bit_HW_SPI(ucg: ucg)
        }
        break
        
    case UCG_COM_MSG_CHANGE_CD_LINE:
        // Not used
        break
        
    case UCG_COM_MSG_SEND_BYTE:
        ucg_com_arduino_send_3wire_9bit_HW_SPI(ucg: ucg, firstBit: ucg.pointee.com_status & UInt8(UCG_COM_STATUS_MASK_CD), data: UInt8(arg))
        break
        
    case UCG_COM_MSG_REPEAT_1_BYTE:
        var arg = arg
        
        while arg > 0 {
            ucg_com_arduino_send_3wire_9bit_HW_SPI(ucg: ucg, firstBit: ucg.pointee.com_status & UInt8(UCG_COM_STATUS_MASK_CD), data: data.pointee)
            arg -= 1
        }
        break
        
    case UCG_COM_MSG_REPEAT_2_BYTES:
        var arg = arg
        
        while arg > 0 {
            ucg_com_arduino_send_3wire_9bit_HW_SPI(ucg: ucg, firstBit: ucg.pointee.com_status & UInt8(UCG_COM_STATUS_MASK_CD), data: data.pointee)
            ucg_com_arduino_send_3wire_9bit_HW_SPI(ucg: ucg, firstBit: ucg.pointee.com_status & UInt8(UCG_COM_STATUS_MASK_CD), data: data.advanced(by: 1).pointee)
            arg -= 1
        }
        break
        
    case UCG_COM_MSG_REPEAT_3_BYTES:
        var arg = arg
        
        while arg > 0 {
            ucg_com_arduino_send_3wire_9bit_HW_SPI(ucg: ucg, firstBit: ucg.pointee.com_status & UInt8(UCG_COM_STATUS_MASK_CD), data: data.pointee)
            ucg_com_arduino_send_3wire_9bit_HW_SPI(ucg: ucg, firstBit: ucg.pointee.com_status & UInt8(UCG_COM_STATUS_MASK_CD), data: data.advanced(by: 1).pointee)
            ucg_com_arduino_send_3wire_9bit_HW_SPI(ucg: ucg, firstBit: ucg.pointee.com_status & UInt8(UCG_COM_STATUS_MASK_CD), data: data.advanced(by: 2).pointee)
            arg -= 1
        }
        break
        
    case UCG_COM_MSG_SEND_STR:
        var arg = arg
        var data = data!
        
        while arg > 0 {
            ucg_com_arduino_send_3wire_9bit_HW_SPI(ucg: ucg, firstBit: ucg.pointee.com_status & UInt8(UCG_COM_STATUS_MASK_CD), data: data.pointee)
            
            data = data.advanced(by: 1)
            arg -= 1
        }
        break
        
    case UCG_COM_MSG_SEND_CD_DATA_SEQUENCE:
        var lastCD = ucg.pointee.com_status & UInt8(UCG_COM_STATUS_MASK_CD)
        var arg = arg
        var data = data!
        
        while arg > 0 {
            if data.pointee != 0 {
                if data.pointee == 1 {
                    lastCD = 0
                } else {
                    lastCD = 1
                }
            }
            
            data = data.advanced(by: 1)
            ucg_com_arduino_send_3wire_9bit_HW_SPI(ucg: ucg, firstBit: lastCD, data: data.pointee)
            data = data.advanced(by: 1)
            arg -= 1
        }
        break
        
    default:
        break
    }
    
    return 1
}

// MARK: - 8-bit Parallel

// **Please read the comment in ucg_com_arduino_init_8bit(ucg:)!!!**
//var u8g_data_port = [DigitalOut?](repeating: nil, count: 9)

func ucg_com_arduino_init_8bit(ucg: UnsafeMutablePointer<ucg_t>!) {
    /*
    Originally, this method was supposed to be used under the precedent that this implementation would map the pins on the virtual bus to registers in memory and would be pulled low/high using simple bitmasks to improve speed.
     
    Because this cannot be done (at least now) on SwiftIO, this function does absolutely nothing.
     
    However, Due to porting concerns, I am still keeping this function in the code as a stub, in case anything calls it or does something stupid.
     
    This was supposed to be the original implementation:
     
    u8g_data_port[0] = pin_list[Int(UCG_PIN_D0)]
    u8g_data_port[1] = pin_list[Int(UCG_PIN_D1)]
    u8g_data_port[2] = pin_list[Int(UCG_PIN_D2)]
    u8g_data_port[3] = pin_list[Int(UCG_PIN_D3)]
    u8g_data_port[4] = pin_list[Int(UCG_PIN_D4)]
    u8g_data_port[5] = pin_list[Int(UCG_PIN_D5)]
    
    u8g_data_port[6] = pin_list[Int(UCG_PIN_D6)]
    u8g_data_port[7] = pin_list[Int(UCG_PIN_D7)]
    
    u8g_data_port[8] = pin_list[Int(UCG_PIN_WR)]*/
}

func ucg_com_arduino_send_8bit(ucg: UnsafeMutablePointer<ucg_t>!, data: UInt8) {
    pin_list[Int(UCG_PIN_D0)]?.write(data & 1 != 0 ? true : false)
    pin_list[Int(UCG_PIN_D1)]?.write(data & 2 != 0 ? true : false)
    pin_list[Int(UCG_PIN_D2)]?.write(data & 4 != 0 ? true : false)
    pin_list[Int(UCG_PIN_D3)]?.write(data & 8 != 0 ? true : false)
    pin_list[Int(UCG_PIN_D4)]?.write(data & 16 != 0 ? true : false)
    pin_list[Int(UCG_PIN_D5)]?.write(data & 32 != 0 ? true : false)
    pin_list[Int(UCG_PIN_D6)]?.write(data & 64 != 0 ? true : false)
    pin_list[Int(UCG_PIN_D7)]?.write(data & 128 != 0 ? true : false)
    wait(us: 1)
    pin_list[Int(UCG_PIN_WR)]?.write(false)
    wait(us: 1)
    pin_list[Int(UCG_PIN_WR)]?.write(true)
}

func ucg_com_arduino_generic_8bit(ucg: UnsafeMutablePointer<ucg_t>!, msg: Int16, arg: UInt16, data: UnsafeMutablePointer<UInt8>!) -> Int16 {
    switch Int32(msg) {
    case UCG_COM_MSG_POWER_UP:
        pin_list[Int(UCG_PIN_CD)]?.write(true)
        pin_list[Int(UCG_PIN_WR)]?.write(true)
        pin_list[Int(UCG_PIN_CS)]?.write(true)
        pin_list[Int(UCG_PIN_RST)]?.write(true)
        
        ucg_com_arduino_init_8bit(ucg: ucg)
        break
        
    case UCG_COM_MSG_POWER_DOWN:
        break
        
    case UCG_COM_MSG_DELAY:
        wait(us: Int(arg))
        break
        
    case UCG_COM_MSG_CHANGE_RESET_LINE:
        pin_list[Int(UCG_PIN_RST)]?.write(arg != 0 ? true : false)
        break
        
    case UCG_COM_MSG_CHANGE_CS_LINE:
        pin_list[Int(UCG_PIN_CS)]?.write(arg != 0 ? true : false)
        break
        
    case UCG_COM_MSG_CHANGE_CD_LINE:
        pin_list[Int(UCG_PIN_CD)]?.write(arg != 0 ? true : false)
        break
        
    case UCG_COM_MSG_SEND_BYTE:
        ucg_com_arduino_send_8bit(ucg: ucg, data: UInt8(arg))
        break
        
    case UCG_COM_MSG_REPEAT_1_BYTE:
        var arg = arg
        
        while arg > 0 {
            ucg_com_arduino_send_8bit(ucg: ucg, data: data.pointee)
            arg -= 1
        }
        break
        
    case UCG_COM_MSG_REPEAT_2_BYTES:
        var arg = arg
        
        while arg > 0 {
            ucg_com_arduino_send_8bit(ucg: ucg, data: data.pointee)
            ucg_com_arduino_send_8bit(ucg: ucg, data: data.advanced(by: 1).pointee)
            arg -= 1
        }
        break
        
    case UCG_COM_MSG_REPEAT_3_BYTES:
        var arg = arg
        
        while arg > 0 {
            ucg_com_arduino_send_8bit(ucg: ucg, data: data.pointee)
            ucg_com_arduino_send_8bit(ucg: ucg, data: data.advanced(by: 1).pointee)
            ucg_com_arduino_send_8bit(ucg: ucg, data: data.advanced(by: 2).pointee)
            arg -= 1
        }
        break
        
    case UCG_COM_MSG_SEND_STR:
        var arg = arg
        var data = data!
        
        while arg > 0 {
            ucg_com_arduino_send_8bit(ucg: ucg, data: data.pointee)
            arg -= 1
            data += 1
        }
        break
        
    case UCG_COM_MSG_SEND_CD_DATA_SEQUENCE:
        var arg = arg
        var data = data!
        
        while arg > 0 {
            if data.pointee != 0 {
                if data.pointee == 1 {
                    pin_list[Int(UCG_PIN_CD)]?.write(false)
                } else {
                    pin_list[Int(UCG_PIN_CD)]?.write(true)
                }
            }
            
            data += 1
            ucg_com_arduino_send_8bit(ucg: ucg, data: data.pointee)
            data += 1
            arg -= 1
        }
        break
        
    default:
        break
    }
    
    return 1
}

// MARK: - 8-bit HW SPI

func ucg_com_arduino_4wire_HW_SPI(ucg: UnsafeMutablePointer<ucg_t>!, msg: Int16, arg: UInt16, data: UnsafeMutablePointer<UInt8>!) -> Int16 {
    switch Int32(msg) {
    case UCG_COM_MSG_POWER_UP:
        //let csPin = pin_list[Int(UCG_PIN_CS)]
        //let rstPin = pin_list[Int(UCG_PIN_RST)]
        
        //SPIHandler = SPI(Id.SPI0, speed: clockSpeed)
        break
        
    case UCG_COM_MSG_POWER_DOWN:
        //spi = nil
        break
        
    case UCG_COM_MSG_DELAY:
        wait(us: Int(arg))
        break
        
    case UCG_COM_MSG_CHANGE_RESET_LINE:
        let rstPin = pin_list[Int(UCG_PIN_RST)]
        
        rstPin?.write(arg != 0 ? true : false)
        break
        
    case UCG_COM_MSG_CHANGE_CS_LINE:
        let csPin = pin_list[Int(UCG_PIN_CS)]
        
        csPin?.write(arg != 0 ? true : false)
        break
        
    case UCG_COM_MSG_CHANGE_CD_LINE:
        pin_list[Int(UCG_PIN_CD)]!.write(arg != 0 ? true : false)
        break
        
    case UCG_COM_MSG_SEND_BYTE:
        SPIHandler?.write(UInt8(arg))
        break
        
    case UCG_COM_MSG_REPEAT_1_BYTE:
        var arg = arg
        
        while arg > 0 {
            SPIHandler.write(data.pointee)
            arg -= 1
        }
        break
        
    case UCG_COM_MSG_REPEAT_2_BYTES:
        var arg = arg
        
        while arg > 0 {
            SPIHandler.write([data.pointee, data.advanced(by: 1).pointee])
            arg -= 1
        }
        break
        
    case UCG_COM_MSG_REPEAT_3_BYTES:
        var arg = arg
        
        while arg > 0 {
            SPIHandler.write([data.pointee, data.advanced(by: 1).pointee, data.advanced(by: 2).pointee])
            arg -= 1
        }
        break
        
    case UCG_COM_MSG_SEND_STR:
        var arg = arg
        var data = data!
        
        while arg > 0 {
            SPIHandler.write(data.pointee)
            
            arg -= 1
            data = data.advanced(by: 1)
        }
        break
        
    case UCG_COM_MSG_SEND_CD_DATA_SEQUENCE:
        var arg = arg
        var data = data!
        let cdPin = pin_list[Int(UCG_PIN_CD)]!
        
        while arg > 0 {
            if data.pointee != 0 {
                if data.pointee == 1 {
                    cdPin.write(false)
                } else {
                    cdPin.write(true)
                }
            }
            
            data = data.advanced(by: 1)
            SPIHandler.write(data.pointee)
            data = data.advanced(by: 1)
            arg -= 1
        }
        break
        
    default:
        break
    }
    
    return 1
}
