import argparse
from pathlib import Path
from dataclasses import dataclass
from typing import List
import jinja2
import ctypes

@dataclass(frozen=True)
class Packet():
    name: str
    packet_id: int

class ProtoParser():
    def __init__(
        self, 
        start_id: int, 
        recv_prefix: str = 'C_', 
        send_prefix: str = 'S_',
    ):
        self.id = start_id
        self.recv_prefix = recv_prefix
        self.send_prefix = send_prefix
        self.packets: List[Packet] = []
        
    def parse(self, proto_path) -> None:
        with proto_path.open("r", encoding="utf-8") as f:
            for line in f.readlines():
                if not line.startswith('message'):
                    continue

                name = line.split()[1].upper()
                if not name.startswith((self.recv_prefix, self.send_prefix)):
                    continue

                packet = Packet(name, self.id)
                self.packets.append(packet)
                self.id += 1

    @property
    def recv_packets(self) -> List[Packet]:
        return [packet for packet in self.packets if packet.name.startswith(self.recv_prefix)]

    @property
    def send_packets(self) -> List[Packet]:
        return [packet for packet in self.packets if packet.name.startswith(self.send_prefix)]

def build_arg_parser() -> argparse.ArgumentParser:
    p = argparse.ArgumentParser(description = "PacketGenerator")
    p.add_argument('--path', "-p", type=Path, 
                   default=Path(r"E:/Github/Unreal-CPP-MMORPG/Server/Common/Protobuf/Protocol.proto"), 
                   help='proto file path')
    p.add_argument('--output', "-o", type=Path, 
                   default=Path('TestPacketHandler.h'), 
                   help='output file')
    p.add_argument('--recv', default='C_', help='recv packet convention')
    p.add_argument('--send', default='S_', help='send packet convention')
    return p

def main():
    args = build_arg_parser().parse_args()

    parser = ProtoParser(start_id=1000, recv_prefix=args.recv, send_prefix=args.send)
    parser.parse(args.path)

    templates_dir = Path(__file__).parent / 'Templates'
    env = jinja2.Environment(
        loader=jinja2.FileSystemLoader(str(templates_dir)),
        autoescape=False,
    )
    template = env.get_template('PacketHandler.h')
    rendered = template.render(parser=parser, output=args.output.stem)

    args.output.write_text(rendered, encoding="utf-8")
    print(rendered)
    return

if __name__ == "__main__":
    main()