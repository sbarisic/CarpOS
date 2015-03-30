using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Drawing;
using System.IO;

namespace PngToCarpic {
	class Program {
		static void Main(string[] args) {
			if (args.Length != 1)
				return;

			string In = args[0];
			string Out = Path.GetFileNameWithoutExtension(In) + ".carpic";
			Convert(In, Out);
		}

		static void Convert(string File, string OutFile) {
			Bitmap Pic = new Bitmap(File);
			FileStream Out = new FileStream(OutFile, FileMode.Create);
			Color Col;

			/*
			Out.WriteByte((byte)(Pic.Size.Width % 256));
			Out.WriteByte((byte)(Pic.Size.Width / 256));
			Out.WriteByte((byte)(Pic.Size.Height % 256));
			Out.WriteByte((byte)(Pic.Size.Height / 256));
			*/

			for (int h = 0; h < Pic.Size.Height; h++) {
				for (int w = 0; w < Pic.Size.Width; w++) {
					Col = Pic.GetPixel(w, h);
					Out.WriteByte(Col.B);
					Out.WriteByte(Col.G);
					Out.WriteByte(Col.R);
					Out.WriteByte(Col.A);
				}
			}
		}
	}
}
