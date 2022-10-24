import SwiftIO
import CUcglibSwiftIO

#if SWIFTIOBOARD
import SwiftIOBoard

#else
import SwiftIOFeather
#endif

func makeUnafeMutablePointer<T>(_ p: UnsafeMutablePointer<T>) -> UnsafeMutablePointer<T> {
    return p
}

fileprivate var pin_list = [DigitalOut?](repeating: nil, count: Int(UCG_PIN_COUNT))

fileprivate var spi: SPI!
fileprivate var clockSpeed = 5000000

public class Ucglib {
    internal var ucg: ucg_t!
    internal var dev_cb: ucg_dev_fnptr!
    internal var ext_cb: ucg_dev_fnptr!
    
    private var tx: ucg_int_t!
    private var ty: ucg_int_t!
    private var tdir: UInt8!
    
    internal func `init`() {
        var i: UInt8! = nil
        
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
    
    public func setClockSpeedHertz(_ speed: Int) {
        clockSpeed = speed
        spi?.setSpeed(clockSpeed)
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
        ucg_Init(&ucg, dev_cb, ext_cb, ucg_com_arduino_illi9325_SW_SPI)
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
    public init(dev: ucg_dev_fnptr, ext: ucg_dev_fnptr, cd: IdName, cs: IdName?, reset: IdName?) {
        super.init()
        
        dev_cb = dev
        ext_cb = ext
        
        pin_list[Int(UCG_PIN_RST)] = reset != nil ? DigitalOut(reset!) : nil
        pin_list[Int(UCG_PIN_CS)] = cs != nil ? DigitalOut(cs!) : nil
        pin_list[Int(UCG_PIN_CD)] = DigitalOut(cd)
    }
    
    public func begin(isTransparent: UInt8) {
        ucg_Init(&ucg, dev_cb, ext_cb, ucg_com_arduino_4wire_HW_SPI)
        ucg_SetFontMode(&ucg, isTransparent)
    }
}

public class Ucglib3Wire9bitHWSPI: Ucglib {
    public init(dev: ucg_dev_fnptr, ext: ucg_dev_fnptr, cs: IdName?, reset: IdName?) {
        super.init()
        
        dev_cb = dev
        ext_cb = ext
        
        pin_list[Int(UCG_PIN_RST)] = reset != nil ? DigitalOut(reset!) : nil
        pin_list[Int(UCG_PIN_CS)] = cs != nil ? DigitalOut(cs!) : nil
    }
    
    public func begin(isTransparent: UInt8) {
        
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
        
    }
}

public class Ucglib_ST7735_18x128x160_HWSPI: Ucglib4WireHWSPI {
    public init(cd: IdName, cs: IdName?, reset: IdName?) {
        super.init(dev: ucg_dev_st7735_18x128x160, ext: ucg_ext_st7735_18, cd: cd, cs: cs, reset: reset)
    }
}

public class Ucglib_ST7735_18x128x160_SWSPI: Ucglib4WireSWSPI {
    public init(scl: IdName, sda: IdName, cd: IdName, cs: IdName?, reset: IdName?) {
        super.init(dev: ucg_dev_st7735_18x128x160, ext: ucg_ext_st7735_18, scl: scl, sda: sda, cd: cd, cs: cs, reset: reset)
    }
}

public class Ucglib_ILI9341_18x240x320_HWSPI: Ucglib4WireHWSPI {
    public init(cd: IdName, cs: IdName?, reset: IdName?) {
        super.init(dev: ucg_dev_ili9341_18x240x320, ext: ucg_ext_ili9341_18, cd: cd, cs: cs, reset: reset)
    }
}

public class Ucglib_ILI9341_18x240x320_SWSPI: Ucglib4WireSWSPI {
    public init(scl: IdName, sda: IdName, cd: IdName, cs: IdName?, reset: IdName?) {
        super.init(dev: ucg_dev_ili9341_18x240x320, ext: ucg_ext_ili9341_18, scl: scl, sda: sda, cd: cd, cs: cs, reset: reset)
    }
}

public class Ucglib_HX8352C_18x240x400_HWSPI: Ucglib4WireHWSPI {
    public init(cd: IdName, cs: IdName?, reset: IdName?) {
        super.init(dev: ucg_dev_hx8352c_18x240x400, ext: ucg_ext_hx8352c_18, cd: cd, cs: cs, reset: reset)
    }
}

public class Ucglib_HX8352C_18x240x400_SWSPI: Ucglib4WireSWSPI {
    public init(scl: IdName, sda: IdName, cd: IdName, cs: IdName?, reset: IdName?) {
        super.init(dev: ucg_dev_hx8352c_18x240x400, ext: ucg_ext_hx8352c_18, scl: scl, sda: sda, cd: cd, cs: cs, reset: reset)
    }
}

public class Ucglib_ILI9486_18x320x480_HWSPI: Ucglib4WireHWSPI {
    public init(cd: IdName, cs: IdName?, reset: IdName?) {
        super.init(dev: ucg_dev_ili9486_18x320x480, ext: ucg_ext_ili9486_18, cd: cd, cs: cs, reset: reset)
    }
}

public class Ucglib_ILI9486_18x320x480_SWSPI: Ucglib4WireSWSPI {
    public init(scl: IdName, sda: IdName, cd: IdName, cs: IdName?, reset: IdName?) {
        super.init(dev: ucg_dev_ili9486_18x320x480, ext: ucg_ext_ili9486_18, scl: scl, sda: sda, cd: cd, cs: cs, reset: reset)
    }
}

public class Ucglib_ILI9163_18x128x128_HWSPI: Ucglib4WireHWSPI {
    public init(cd: IdName, cs: IdName?, reset: IdName?) {
        super.init(dev: ucg_dev_ili9163_18x128x128, ext: ucg_ext_ili9163_18, cd: cd, cs: cs, reset: reset)
    }
}

public class Ucglib_ILI9163_18x128x128_SWSPI: Ucglib4WireSWSPI {
    public init(scl: IdName, sda: IdName, cd: IdName, cs: IdName?, reset: IdName?) {
        super.init(dev: ucg_dev_ili9163_18x128x128, ext: ucg_ext_ili9163_18, scl: scl, sda: sda, cd: cd, cs: cs, reset: reset)
    }
}

public class Ucglib_SSD1351_18x128x128_HWSPI: Ucglib4WireHWSPI {
    public init(cd: IdName, cs: IdName?, reset: IdName?) {
        super.init(dev: ucg_dev_ssd1351_18x128x128_ilsoft, ext: ucg_ext_ssd1351_18, cd: cd, cs: cs, reset: reset)
    }
}

public class Ucglib_SSD1351_18x128x128_SWSPI: Ucglib4WireSWSPI {
    public init(scl: IdName, sda: IdName, cd: IdName, cs: IdName?, reset: IdName?) {
        super.init(dev: ucg_dev_ssd1351_18x128x128_ilsoft, ext: ucg_ext_ssd1351_18, scl: scl, sda: sda, cd: cd, cs: cs, reset: reset)
    }
}

public class Ucglib_SSD1351_18x128x128_FT_HWSPI: Ucglib4WireHWSPI {
    public init(cd: IdName, cs: IdName?, reset: IdName?) {
        super.init(dev: ucg_dev_ssd1351_18x128x128_ft, ext: ucg_ext_ssd1351_18, cd: cd, cs: cs, reset: reset)
    }
}

public class Ucglib_SSD1351_18x128x128_FT_SWSPI: Ucglib4WireSWSPI {
    public init(scl: IdName, sda: IdName, cd: IdName, cs: IdName?, reset: IdName?) {
        super.init(dev: ucg_dev_ssd1351_18x128x128_ft, ext: ucg_ext_ssd1351_18, scl: scl, sda: sda, cd: cd, cs: cs, reset: reset)
    }
}

public class Ucglib_PCF8833_16x132x132_HWSPI: Ucglib3Wire9bitHWSPI {
    public init(cs: IdName?, reset: IdName?) {
        super.init(dev: ucg_dev_pcf8833_16x132x132, ext: ucg_ext_pcf8833_16, cs: cs, reset: reset)
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
    public init(cd: IdName, cs: IdName?, reset: IdName?) {
        super.init(dev: ucg_dev_ssd1331_18x96x64_univision, ext: ucg_ext_ssd1331_18, cd: cd, cs: cs, reset: reset)
    }
}

public class Ucglib_SSD1331_18x96x64_UNIVISION_SWSPI: Ucglib4WireSWSPI {
    public init(scl: IdName, sda: IdName, cd: IdName, cs: IdName?, reset: IdName?) {
        super.init(dev: ucg_dev_ssd1331_18x96x64_univision, ext: ucg_ext_ssd1331_18, scl: scl, sda: sda, cd: cd, cs: cs, reset: reset)
    }
}

public class Ucglib_SEPS225_16x128x128_UNIVISION_HWSPI: Ucglib4WireHWSPI {
    public init(cd: IdName, cs: IdName?, reset: IdName?) {
        super.init(dev: ucg_dev_seps225_16x128x128_univision, ext: ucg_ext_seps225_16, cd: cd, cs: cs, reset: reset)
    }
}

public class Ucglib_SEPS225_16x128x128_UNIVISION_SWSPI: Ucglib4WireSWSPI {
    public init(scl: IdName, sda: IdName, cd: IdName, cs: IdName?, reset: IdName?) {
        super.init(dev: ucg_dev_seps225_16x128x128_univision, ext: ucg_ext_seps225_16, scl: scl, sda: sda, cd: cd, cs: cs, reset: reset)
    }
}


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

func ucg_com_arduino_illi9325_SW_SPI(ucg: UnsafeMutablePointer<ucg_t>!, msg: Int16, arg: UInt16, data: UnsafeMutablePointer<UInt8>!) -> Int16 {
    switch Int32(msg) {
    default:
        break
    }
    
    return 1
}

func ucg_com_arduino_send_3wire_9bit_SW_SPI(ucg: UnsafeMutablePointer<ucg_t>!, firstBit: UInt8, data: UInt8) {
    
}

public func ucg_com_arduino_3wire_9bit_SW_SPI(ucg: UnsafeMutablePointer<ucg_t>!, msg: Int16, arg: UInt16, data: UnsafeMutablePointer<UInt8>!) -> Int16 {
    switch Int32(msg) {
    default:
        break
    }
    
    return 1
}

// TODO: - 9-bit HW SPI

// TODO: - 8-bit Parallel

func ucg_com_arduino_4wire_HW_SPI(ucg: UnsafeMutablePointer<ucg_t>!, msg: Int16, arg: UInt16, data: UnsafeMutablePointer<UInt8>!) -> Int16 {
    switch Int32(msg) {
    case UCG_COM_MSG_POWER_UP:
        //let csPin = pin_list[Int(UCG_PIN_CS)]
        let rstPin = pin_list[Int(UCG_PIN_RST)]
        
        spi = SPI(Id.SPI0, speed: clockSpeed)
        break
        
    case UCG_COM_MSG_POWER_DOWN:
        spi = nil
        break
        
    case UCG_COM_MSG_DELAY:
        wait(us: Int(arg))
        break
        
    case UCG_COM_MSG_CHANGE_RESET_LINE:
        let rstPin = pin_list[Int(UCG_PIN_RST)]
        
        rstPin?.write(msg != 0 ? true : false)
        break
        
    case UCG_COM_MSG_CHANGE_CS_LINE:
        let csPin = pin_list[Int(UCG_PIN_CS)]
        
        csPin?.write(msg != 0 ? true : false)
        break
        
    case UCG_COM_MSG_CHANGE_CD_LINE:
        pin_list[Int(UCG_PIN_CD)]!.write(msg != 0 ? true : false)
        break
        
    case UCG_COM_MSG_SEND_BYTE:
        spi?.write(UInt8(arg))
        break
        
    case UCG_COM_MSG_REPEAT_1_BYTE:
        var arg = arg
        
        while arg > 0 {
            spi.write(data.pointee)
            arg -= 1
        }
        break
        
    case UCG_COM_MSG_REPEAT_2_BYTES:
        var arg = arg
        
        while arg > 0 {
            spi.write([data.pointee, data.advanced(by: 1).pointee])
            arg -= 1
        }
        break
        
    case UCG_COM_MSG_REPEAT_3_BYTES:
        var arg = arg
        
        while arg > 0 {
            spi.write([data.pointee, data.advanced(by: 1).pointee, data.advanced(by: 2).pointee])
            arg -= 1
        }
        break
        
    case UCG_COM_MSG_SEND_STR:
        var arg = arg
        var data = data!
        
        while arg > 0 {
            spi.write(data.pointee)
            
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
            spi.write(data.pointee)
            data = data.advanced(by: 1)
            arg -= 1
        }
        break
        
    default:
        break
    }
    
    return 1
}
