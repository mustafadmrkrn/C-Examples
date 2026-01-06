import processing.serial.*; // Seri haberleşme kütüphanesi
import java.awt.event.KeyEvent; 
import java.io.IOException;

Serial myPort; // Serial nesnesi

// --- Değişkenler ---
String angle="";
String distance="";
String data="";
String noObject;
float pixsDistance;
int iAngle, iDistance;
int index1=0;
int index2=0;
PFont orcFont;

// --- Ölçekleme için EKLENEN Değişkenler ---
float centerX, centerY; // Radarımızın merkez koordinatları
float maxRadius;        // Radarın maksimum piksel yarıçapı
float maxDist = 40.0;   // Sensörün cm cinsinden maksimum mesafesi (kodunuza göre)
float scaleFactor;      // Cm'yi piksele dönüştürme katsayısı (22.5'in yerine)
float textBarY;         // Alttaki metin çubuğunun Y konumu

void setup() {
  size(1280, 720);
  // Veya tam ekran için:
  // fullScreen();
  smooth();
  
  try {
    myPort = new Serial(this,"COM4", 9600);
    myPort.bufferUntil('.'); 
  } catch (Exception e) {
    println("Seri port (COM4) bulunamadı veya açılamadı.");
    println("Lütfen Arduino'nun bağlı olduğundan ve doğru portu seçtiğinizden emin olun.");
    e.printStackTrace();
    exit(); // Port yoksa programı durdur
  }
  
  // --- Font Ayarı ---
  // .vlw fontları ölçeklenmez. 
  // Bunun yerine sisteminizdeki bir fontu kullanmak daha esnektir.
  // orcFont = loadFont("Arial-Black-48.vlw"); 
  orcFont = createFont("Arial Black", 48, true); // Arial Black fontu oluştur (ölçeklenebilir)
  textFont(orcFont);

  // --- EKLENDİ: Dinamik Ölçekleme Ayarları ---
  // Bu hesaplamalar 'size()' komutundan SONRA yapılmalıdır.
  centerX = width / 2;                // Ekranın yatay ortası
  centerY = height * 0.9;             // Pivot noktası ekranın %90 aşağısında
  textBarY = centerY;                 // Metin çubuğu pivotun altında başlasın
  
  // Maksimum yarıçapı, ekranın en dar boyutuna göre ayarla
  // (Yatayda width/2'den, dikeyde centerY'den büyük olamaz)
  // %95'ini alarak kenarlarda biraz boşluk bırakıyoruz.
  maxRadius = min(width / 2, centerY) * 0.95; 
  
  // Ölçek faktörünü hesapla (örn. 900 piksel / 40 cm = 22.5 piksel/cm)
  scaleFactor = maxRadius / maxDist; 
}

void draw() {
  
  // Arka planı siyaha boyamak yerine, motion blur için soluk bir katman çizin
  // Orijinal kodunuzdaki rect(0, 0, width, 1010) yerine:
  noStroke();
  fill(0, 4); // Düşük opaklıkta siyah
  rect(0, 0, width, textBarY); // Sadece radar alanını soldur (metinleri değil)
  
  // Alttaki metin çubuğunu temizle
  fill(0); // Tam siyah
  rect(0, textBarY, width, height);
  
  fill(98, 245, 31); // Varsayılan çizim rengi (yeşil)

  // Radar fonksiyonlarını çağır
  drawRadar();
  drawLine();
  drawObject();
  drawText();
}

void serialEvent (Serial myPort) { 
  data = myPort.readStringUntil('.');
  if (data == null) return; // Veri yoksa çık
  
  data = data.substring(0, data.length()-1);
  
  index1 = data.indexOf(","); 
  if (index1 == -1) return; // Virgül bulunamazsa (hatalı veri) çık
    
  angle = data.substring(0, index1); 
  distance = data.substring(index1+1, data.length()); 
  
  // Veri dönüştürme (try-catch ile sarmalayarak çökme engellendi)
  try {
    iAngle = Integer.parseInt(angle);
    iDistance = Integer.parseInt(distance);
  } catch (NumberFormatException e) {
    println("Hatalı seri veri alındı: " + data);
  }
  
  // Açıyı 0-180 arası sınırla (sensör genellikle bu aralıkta çalışır)
  iAngle = constrain(iAngle, 0, 180); 
}

void drawRadar() {
  pushMatrix();
  translate(centerX, centerY); // Dinamik merkeze git
  
  noFill();
  strokeWeight(2);
  stroke(98, 245, 31);
  
  // Arc'ları (yayları) ölçeklenmiş çiz
  // Orijinal kodunuzdaki 1800, 1400 vb. yerine scaleFactor kullanıldı
  // 10cm, 20cm, 30cm ve 40cm (maxDist) için yaylar
  arc(0, 0, (10 * scaleFactor) * 2, (10 * scaleFactor) * 2, PI, TWO_PI);
  arc(0, 0, (20 * scaleFactor) * 2, (20 * scaleFactor) * 2, PI, TWO_PI);
  arc(0, 0, (30 * scaleFactor) * 2, (30 * scaleFactor) * 2, PI, TWO_PI);
  arc(0, 0, maxRadius * 2, maxRadius * 2, PI, TWO_PI); // 40 cm = maxRadius
  
  // Açı çizgilerini maxRadius'e göre ölçeklenmiş çiz
  line(-maxRadius, 0, maxRadius, 0); // 0-180 derece hattı
  line(0, 0, -maxRadius * cos(radians(30)), -maxRadius * sin(radians(30)));
  line(0, 0, -maxRadius * cos(radians(60)), -maxRadius * sin(radians(60)));
  line(0, 0, -maxRadius * cos(radians(90)), -maxRadius * sin(radians(90)));
  line(0, 0, -maxRadius * cos(radians(120)), -maxRadius * sin(radians(120)));
  line(0, 0, -maxRadius * cos(radians(150)), -maxRadius * sin(radians(150)));
  
  popMatrix();
}

void drawObject() {
  pushMatrix();
  translate(centerX, centerY); // Dinamik merkeze git
  strokeWeight(9);
  stroke(255, 10, 10); // Kırmızı
  
  // Mesafeyi cm'den piksele dönüştür (Dinamik scaleFactor ile)
  pixsDistance = iDistance * scaleFactor; 
  
  // Limiti hardcoded 40 yerine maxDist değişkeni ile kontrol et
  if(iDistance < maxDist){
    // Orijinal kodunuzdaki 950 yerine maxRadius kullanıldı
    // Bu çizgi, nesnenin olduğu yerden radarın dışına doğru bir iz bırakır
    line(pixsDistance * cos(radians(iAngle)), -pixsDistance * sin(radians(iAngle)), 
         maxRadius * cos(radians(iAngle)),    -maxRadius * sin(radians(iAngle)));
  }
  popMatrix();
}

void drawLine() {
  pushMatrix();
  strokeWeight(9);
  stroke(30, 250, 60);
  translate(centerX, centerY); // Dinamik merkeze git
  
  // Tarama çizgisini maxRadius'e göre çiz (950 yerine)
  line(0, 0, maxRadius * cos(radians(iAngle)), -maxRadius * sin(radians(iAngle)));
  popMatrix();
}

void drawText() { 
  pushMatrix();
  
  // --- Metin Durumu ---
  if(iDistance > maxDist) { // maxDist değişkenini kullan
    noObject = "Menzil Dışı";
  } else {
    noObject = "Menzilde";
  }
  
  // --- Dinamik Metin Boyutları ---
  float largeText = max(24, width / 50); // Ekran genişliğine göre (min 24)
  float smallText = max(12, width / 80); // Ekran genişliğine göre (min 12)

  fill(98, 245, 31); // Yeşil metin
  
  // --- Alttaki Bilgi Metinleri ---
  float textY = textBarY + (height - textBarY) / 2; // Metin çubuğunun dikey ortası
  
  textSize(largeText);
  textAlign(LEFT, CENTER); // Sola hizalı, dikeyde ortalı
  text("Durum: " + noObject, width * 0.1, textY);
  text("Açı: " + iAngle + " °", width * 0.45, textY);
  text("Mesafe: ", width * 0.7, textY);
  if(iDistance < maxDist) {
    text(" " + iDistance + " cm", width * 0.7 + textWidth("Mesafe: "), textY);
  }

  // --- Radar Üzerindeki Metinler (Ölçekli) ---
  textSize(smallText);
  textAlign(CENTER, CENTER); // Ortaya hizalı
  
  // Cm etiketlerini ölçekli konumlara yerleştir
  text("10cm", centerX + (10 * scaleFactor), centerY - 20);
  text("20cm", centerX + (20 * scaleFactor), centerY - 20);
  text("30cm", centerX + (30 * scaleFactor), centerY - 20);
  text("40cm", centerX + (40 * scaleFactor), centerY - 20);
  
  // Açı etiketlerini ölçekli konumlara yerleştir (döndürerek)
  // Orijinal kodunuzdaki karmaşık hesaplamalar yerine
  // (maxRadius + boşluk) * açı kullanarak yerleştiriyoruz.
  float labelOffset = smallText * 2.5; // Metin için etiket boşluğu
  
  // 30°
  pushMatrix();
  translate(centerX + (maxRadius + labelOffset) * cos(radians(30)), 
            centerY - (maxRadius + labelOffset) * sin(radians(30)));
  rotate(radians(90 - 30)); // Metni okunabilir tutmak için döndür
  text("30°", 0, 0);
  popMatrix();
  
  // 60°
  pushMatrix();
  translate(centerX + (maxRadius + labelOffset) * cos(radians(60)), 
            centerY - (maxRadius + labelOffset) * sin(radians(60)));
  rotate(radians(90 - 60));
  text("60°", 0, 0);
  popMatrix();
  
  // 90°
  pushMatrix();
  translate(centerX + (maxRadius + labelOffset) * cos(radians(90)), 
            centerY - (maxRadius + labelOffset) * sin(radians(90)));
  rotate(radians(90 - 90));
  text("90°", 0, 0);
  popMatrix();
  
  // 120°
  pushMatrix();
  translate(centerX + (maxRadius + labelOffset) * cos(radians(120)), 
            centerY - (maxRadius + labelOffset) * sin(radians(120)));
  rotate(radians(90 - 120));
  text("120°", 0, 0);
  popMatrix();

  // 150°
  pushMatrix();
  translate(centerX + (maxRadius + labelOffset) * cos(radians(150)), 
            centerY - (maxRadius + labelOffset) * sin(radians(150)));
  rotate(radians(90 - 150));
  text("150°", 0, 0);
  popMatrix();
  
  popMatrix(); // drawText() başındaki ilk pushMatrix için
}
