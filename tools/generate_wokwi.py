"""Generate both wiring views and a passive custom-chip asset (not a DHT model)."""
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def write_json(path, value):
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(value, indent=2) + '\n', encoding='utf-8')


def diagram(full):
    parts = []
    wires = []

    def part(kind, name, x, y, **attrs):
        parts.append(dict(type=kind, id=name, top=y, left=x, attrs=attrs))

    def text(name, x, y, value):
        part('wokwi-text', name, x, y, text=value)

    def wire(a, b, color, route=None):
        wires.append([a, b, color, route or []])

    part('board-esp32-devkit-c-v4', 'esp', -220, 30)
    part('wokwi-breadboard', 'bb', 30, 100)
    part('chip-dht11-wiring', 'dht', 130, -110)
    text('title', -220, -220, 'SMART INCUBATOR | ' + ('OLED + 5 BUTTONS' if full else 'DHT11 WIRING ONLY'))
    text('legend', -220, -185, 'RED = 3.3V | BLACK = GND | GREEN = DHT11 S / GPIO4')
    text('dhtLabel', 100, -65, 'DHT11 WIRING PLACEHOLDER ONLY\nS = signal | VCC = physical + | GND = physical -\nNO sensor protocol or measurements are simulated')
    text('railLabel', 80, 55, 'TOP + rail = 3.3V | TOP - rail = GND\nUse LEFT rail section only; bottom/right rails unused')
    text('boardLabel', -260, 350, '38-pin DevKitC V4 model\nMatch GPIO labels, NOT header positions\nNo 5V/VIN or output hardware connected')
    wire('esp:3V3', 'bb:tp.1', 'red', ['h-30', 'v-40'])
    wire('esp:GND.1', 'bb:tn.1', 'black', ['h-50', 'v110'])
    wire('dht:VCC', 'bb:tp.3', 'red')
    wire('dht:GND', 'bb:tn.3', 'black')
    wire('esp:4', 'dht:S', 'green', ['h30', 'v-190'])
    wire('esp:TX', '$serialMonitor:RX', '')
    wire('esp:RX', '$serialMonitor:TX', '')
    if full:
        part('board-ssd1306', 'oled', 510, -130, i2cAddress='0x3c')
        text('oledLabel', 490, -20, 'OLED 128x64 SSD1306 @ 0x3C\nBLUE SDA = GPIO21 | YELLOW SCL = GPIO22\nVCC = 3.3V | GND = ground')
        wire('oled:VCC', 'bb:tp.6', 'red')
        wire('oled:GND', 'bb:tn.6', 'black')
        wire('esp:21', 'oled:SDA', 'blue', ['h50', 'v-160'])
        wire('esp:22', 'oled:SCL', 'yellow', ['h70', 'v-180'])
        for i, (name, gpio, color) in enumerate([
            ('MENU', 18, 'orange'), ('UP', 19, 'purple'),
            ('DOWN', 23, 'cyan'), ('OK', 16, 'magenta'), ('TURN', 17, 'brown')]):
            bid = name.lower()
            x = 60 + i * 135
            part('wokwi-pushbutton', bid, x, 410, color=color, label=name, xray='1')
            text(bid + 'Label', x - 10, 485, f'{name} GPIO{gpio}\n1.l = signal\n2.l = GND')
            wire(f'esp:{gpio}', f'{bid}:1.l', color, [f'h{90+i*20}', f'v{180+i*15}'])
            wire(f'{bid}:2.l', f'bb:tn.{8+i}', 'black')
        text('buttonsNote', 60, 560, 'Buttons: INPUT_PULLUP, pressed = LOW\nContacts 1 and 2 must differ; never connect a button to 3.3V\nTURN remains simulated; no motor is present')
    return dict(version=1, author='iamHarinda', editor='wokwi', parts=parts,
                connections=wires, dependencies={})


def main():
    # Exact WASM encoding of: (module (func (export "chip_init"))).
    # No imports, GPIO configuration, pulse generation, or sensor emulation.
    wasm = bytes.fromhex('0061736d0100000001040160000003020100070d0109636869705f696e697400000a040102000b')
    chip = dict(name='DHT11 WIRING ONLY (no emulation)', author='iamHarinda',
                pins=['S', 'VCC', 'GND', '', '', ''])
    for folder, full, prefix in [(ROOT, False, ''), (ROOT / 'wokwi/full', True, '../../')]:
        write_json(folder / 'diagram.json', diagram(full))
        write_json(folder / 'dht11-wiring.chip.json', chip)
        (folder / 'dht11-wiring.chip.wasm').write_bytes(wasm)
        (folder / 'wokwi.toml').write_text(
            '[wokwi]\nversion = 1\n'
            f'firmware = "{prefix}.pio/build/esp32dev/firmware.bin"\n'
            f'elf = "{prefix}.pio/build/esp32dev/firmware.elf"\n\n'
            '[[chip]]\nname = "dht11-wiring"\n'
            'binary = "dht11-wiring.chip.wasm"\n', encoding='utf-8')
    print('Generated two diagrams and passive chip assets; simulation was not run.')


if __name__ == '__main__':
    main()
