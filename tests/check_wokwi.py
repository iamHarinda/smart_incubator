"""Check actual wiring nets against firmware assignments (not an electrical test)."""
import json
import re
import tomllib
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
config = (ROOT / 'include/config.h').read_text()
pins = dict((name, int(value)) for name, value in
            re.findall(r'constexpr uint8_t (\w+_PIN) = (\d+);', config))


def check(folder, full):
    diagram = json.loads((folder / 'diagram.json').read_text())
    parts = {p['id']: p for p in diagram['parts']}
    assert len(parts) == len(diagram['parts']), 'Duplicate part IDs'
    assert parts['esp']['type'] == 'board-esp32-devkit-c-v4'
    assert parts['dht']['type'] == 'chip-dht11-wiring', 'No DHT22 substitution'
    allowed = {
        'esp': {'3V3', 'GND.1', 'TX', 'RX', '4', '21', '22', '18', '19', '23', '16', '17'},
        'bb': {f'{rail}.{n}' for rail in ('tp', 'tn') for n in range(1, 13)},
        'dht': {'S', 'VCC', 'GND'}, '$serialMonitor': {'RX', 'TX'},
    }
    if full:
        allowed['oled'] = {'VCC', 'GND', 'SDA', 'SCL'}
        assert parts['oled']['attrs']['i2cAddress'] == '0x3c'
        for button in ('menu', 'up', 'down', 'ok', 'turn'):
            allowed[button] = {'1.l', '2.l'}
    assert all(p['type'] in {'board-esp32-devkit-c-v4', 'wokwi-breadboard',
                            'chip-dht11-wiring', 'wokwi-text',
                            'board-ssd1306', 'wokwi-pushbutton'}
               for p in parts.values()), 'Unexpected hardware'
    parent = {}

    def find(x):
        parent.setdefault(x, x)
        if parent[x] != x:
            parent[x] = find(parent[x])
        return parent[x]

    def join(a, b):
        parent[find(a)] = find(b)

    # Only the left top rail sections are used. No continuity assumed for any
    # other rail section, or between the + and - rails.
    for rail in ('tp', 'tn'):
        for n in range(2, 13):
            join(f'bb:{rail}.1', f'bb:{rail}.{n}')
    for a, b, color, route in diagram['connections']:
        for endpoint in (a, b):
            part, pin = endpoint.split(':')
            assert part in allowed and pin in allowed[part], endpoint
        assert isinstance(route, list)
        if ':tp.' in a or ':tp.' in b:
            assert color == 'red', '3.3V wires must be red'
        if ':tn.' in a or ':tn.' in b:
            assert color == 'black', 'GND wires must be black'
        join(a, b)
    assert find('esp:3V3') != find('esp:GND.1'), 'Power short'
    assert find('dht:VCC') == find('esp:3V3')
    assert find('dht:GND') == find('esp:GND.1')
    assignments = [('dht:S', 'DHT_DATA_PIN')]
    if full:
        assert find('oled:VCC') == find('esp:3V3')
        assert find('oled:GND') == find('esp:GND.1')
        assignments += [('oled:SDA', 'OLED_SDA_PIN'), ('oled:SCL', 'OLED_SCL_PIN')]
        for name in ('menu', 'up', 'down', 'ok', 'turn'):
            assignments.append((name + ':1.l', name.upper() + '_BUTTON_PIN'))
            assert find(name + ':2.l') == find('esp:GND.1')
    signal_nets = []
    for endpoint, setting in assignments:
        net = find(endpoint)
        assert net == find('esp:' + str(pins[setting])), setting
        assert net not in (find('esp:3V3'), find('esp:GND.1')), 'Signal short'
        signal_nets.append(net)
    assert len(set(signal_nets)) == len(signal_nets), 'Signals shorted together'
    toml = tomllib.loads((folder / 'wokwi.toml').read_text())
    for key in ('firmware', 'elf'):
        assert (folder / toml['wokwi'][key]).is_file(), 'Build artifact missing'
    assert (folder / toml['chip'][0]['binary']).is_file()
    chip = json.loads((folder / 'dht11-wiring.chip.json').read_text())
    assert chip['pins'][:3] == ['S', 'VCC', 'GND']
    print(f'PASS: {folder.relative_to(ROOT) or "."}: pin mapping, power nets, button contacts, assets, TOML')


check(ROOT, False)
check(ROOT / 'wokwi/full', True)
print('Wiring checks passed; no simulation or electrical validation performed.')
