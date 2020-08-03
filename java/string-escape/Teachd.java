import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import org.apache.commons.text.StringEscapeUtils;

public class Teachd {

	public static void main(String[] args) {

		try {
			System.out.println();

			String es0 = new String(Files.readAllBytes(Paths.get("homework0.txt")));
			System.out.println("es0 = "+es0);
			System.out.println();

			String es1 = StringEscapeUtils.escapeJava(es0);
			System.out.println("es1 = "+es1);
			System.out.println();
			
			String es2 = StringEscapeUtils.escapeJava(es1);
			System.out.println("es2 = "+es2);
			System.out.println();
			
		}
		catch (IOException e) {
		}
	}
}
