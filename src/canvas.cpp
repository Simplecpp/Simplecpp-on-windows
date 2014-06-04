    #include <simplecpp>
    #include <cstring>
    #include <cstdlib>
    #include <graphics.h>

    using namespace std;
  
    int screen_number=999;
    int screen_width;
    int screen_height;
    int mouse_x,mouse_y; // mouse cordinates
    int ButtonPress,ButtonRelease,MotionNotify,KeyPress;
    char chr;
    int curr_d; // curr_d stores the current screen id
    int screenBG=2; // background screen used to draw 
    int screenTmp=1; // Temp screen used to add sprite(s) at their position
    unsigned int size;
    void *image;  // pointer to screen buffers

    struct LtSprite
    {
      bool operator()(const Sprite* s1, const Sprite* s2) const {
        return s1->getZIndex() < s2->getZIndex();
      }
    };

    namespace canvas{
      double randuv(double u, double v){
        return u + (v-u)*rand()/(1.0+RAND_MAX);
      }

      void srand(unsigned int seed){
        std::srand(seed);
      }

      void abort(){ std::abort(); }

      int canvas_width(){
        return screen_width;
      }
      int canvas_height(){
        return screen_height;
      }

      std::multiset<Sprite *, LtSprite> s_sprite;
      
      /* waiting time before terminating canvas*/
      void wait(float duration){
        delay((int) (duration * 1000));
      }


      int initCanvas(const char window_title[], int width, int height){

        screen_number=initwindow(width,height, window_title, 0,0); // default screen number '0'
        setmousequeuestatus(WM_LBUTTONUP);
        setmousequeuestatus(WM_LBUTTONDOWN);
        setmousequeuestatus(WM_MOUSEMOVE);
        setbkcolor(COLOR(255,255,255));
        cleardevice();  // clears the current screen to white
        setcolor(COLOR(0,0,0));
        setlinestyle(SOLID_LINE, 1, 2);
        if(width == -1){
          
          screen_width = (getwindowwidth( ) -100);
          screen_height = (getwindowheight( ) -100);
        }
        else{
          screen_width = width;
          screen_height= height;
        }

   /* copying current screen to background screen*/
   /* Now background screen is white and ready for drawing*/
   
    	setactivepage(screen_number); 
    	size = imagesize(0,0,screen_width, screen_height);
    	image = malloc(size);
    	getimage(0,0,screen_width, screen_height, image);
    	setactivepage(screenBG);
    	putimage(0, 0, image, COPY_PUT);
    	setcolor(COLOR(0,0,0));
        
    	curr_d=screen_number;

    	free(image);
       		return 0;
      }

      void closeCanvas(){
       
        closegraph( ); 
        s_sprite.clear();    // Delete all elements from set
      }


      void drawLine(XPoint start, XPoint end, Color line_color, unsigned int line_width){
        setactivepage(curr_d);
        setcolor(line_color);
        setlinestyle(SOLID_LINE, 1, line_width);
        line(start.x, start.y, end.x, end.y);
      }

      void imprintLine(short x1, short y1, short x2, short y2, Color line_color, unsigned int line_width){
        setactivepage(curr_d);
        setcolor(line_color);
        setlinestyle(SOLID_LINE, 1, line_width);
        line(x1, y1, x2, y2);
        setcolor(COLOR(0,0,0));
      }

      void drawPoint(XPoint point, Color point_color){ 

        setactivepage(curr_d);
        setcolor(point_color);
        putpixel(point.x, point.y, point_color);
        setcolor(COLOR(0,0,0));
      }

      void drawCircle(XPoint centre, int radius, Color fill_color, bool fill, unsigned int line_width, int line_style){ 
        // Check fill
        if(fill){
          line_width = radius;
          radius /= 2;
        }
        setcolor(fill_color);
        setlinestyle(line_style, 1, line_width);
        arc(centre.x-radius, centre.y - radius, 0, 360, radius);
        setcolor(COLOR(0,0,0));
      }


      void drawEllipse(XPoint centre, int width, int height, Color fill_color, bool fill, unsigned int line_width, int line_style){ 
        setactivepage(curr_d);
        setfillstyle(SOLID_FILL, fill_color);
        setlinestyle(line_style, 1, line_width);
        setcolor(fill_color);
        
        if(fill)
          fillellipse(centre.x , centre.y , width/2, height/2);
        else
          ellipse(centre.x , centre.y , 0, 360, width/2, height/2);
        setcolor(COLOR(0,0,0));
      }

      void drawPolygon(XPoint *points, int npoints, Color fill_color, bool fill, unsigned int line_width, int line_style){ 
        setactivepage(curr_d);
        setfillstyle(SOLID_FILL, fill_color);
        setlinestyle(line_style, 1, line_width);
        setcolor(fill_color);
        if(fill){ 
    	   int poits[npoints*2];
    	   for(int i=0,j=0;i<npoints;i++){
                poits[j++]=points[i].x;
                poits[j++]=points[i].y;
                }
    	        fillpoly(npoints, poits);
        }
        else{
          XPoint pts[npoints + 1];
          int iter;

          // Create array with closed list
          for(iter = 0; iter < npoints; iter ++){
    			pts[iter] = points[iter];
          }
          pts[iter] = points[0];
          //converting 2d array(XPoint type) to 1d array
          int poits[(npoints+1)*2];
    	      for(int i=0,j=0;i<npoints+1;i++)
    	      {
                poits[j++]=pts[i].x;
                poits[j++]=pts[i].y;
                }
    	        drawpoly(npoints+1, poits);
        }
        setcolor(COLOR(0,0,0));
      }

      void addSprite(Sprite *t){
        if(t){
          s_sprite.insert(t);
        }
      }

      void removeSprite(Sprite *t){
        if(t){
          for(std::multiset<Sprite *>::iterator iter = s_sprite.begin();
    	  iter != s_sprite.end(); iter ++){
    	if((*iter) == t){s_sprite.erase(iter); break;}
          }
        }
        repaint();
      }

      void c_imprint(Sprite* s){
        int temp;
        temp = curr_d;
        curr_d= screenBG;
        s->paint();
        curr_d=temp;
      }


      bool globalRepaintFlag = true;
      
      void beginFrame(){
        globalRepaintFlag = false;
      }
      
      void endFrame(){
        globalRepaintFlag = true;
        repaint();
      }


      void repaint(){
        if(screen_number==999){ 
          cout << "You must first call initCanvas before "<<
    	"using any graphics features.\n";
    	wait(5);
          exit(1);
        }

        if(globalRepaintFlag){
          int tl_x=0,tl_y=0,width=canvas_width(),height=canvas_height();
          
            setactivepage(screenBG);
            size = imagesize(0, 0,canvas_width(), canvas_height());
            image = malloc(size);
            getimage(tl_x, tl_y,width, height, image);
            setactivepage(screenTmp);
            putimage(tl_x, tl_y, image, COPY_PUT);

         
            curr_d=screenTmp;
          	for(std::multiset<Sprite *>::iterator iter = s_sprite.begin(); iter != s_sprite.end(); iter ++){
    			(*iter)->paint();
          }

         
        setactivepage(screenTmp);
        getimage(tl_x, tl_y,width, height, image);
        setactivepage(screen_number);
        putimage(tl_x, tl_y, image, COPY_PUT);
        free(image);
        curr_d=screen_number;
        }
      }


      int textWidth(char op){
        string s;
        s= op;
        return  textWidth(s);
      }

      int textWidth(string text){
       
        char txt[text.size() + 1];
        strcpy(txt, text.c_str());
        return textwidth(txt);
      }
      int textHeight(){
       
        return 16; // default height 16
      }
      int textDescent(){
       
        return 0;
      }

      void drawText(Position position, string text, Color clr){
        XPoint p;
        p.x =(int) position.getX(); p.y = (int) position.getY();
        drawText(p, text.c_str(), clr);
      }

      void drawText(float x, float y, string text, Color clr){
        XPoint p;
        p.x = (int)x; p.y = (int)y;
        drawText(p, text.c_str(), clr);
      }


      void drawText(XPoint position, string message, Color clr){
        setactivepage(curr_d);
        char *cstr = new char [message.size()+1];
        strcpy (cstr, message.c_str());
        setcolor(clr);
    	outtextxy(position.x-textWidth(message)/2, position.y -textHeight()/2 - textDescent(), cstr);
        setcolor(COLOR(0,0,0));
      }

      void drawText(XPoint position, const char* text, Color clr){
       
     	setactivepage(curr_d);
        setcolor(clr);
        outtextxy(position.x, position.y, (char*)text);
    	setcolor(COLOR(0,0,0));
      }

      void spriteStatus(){
        cout<<"Count: "<<s_sprite.size()<<endl;
        for(std::multiset<Sprite *>::iterator iter = s_sprite.begin(); iter != s_sprite.end(); iter ++){
          cout<<"["<<"]-->"<<(*iter)<<endl;
        }
      }


        bool Button_Down = false;
        void Up_handler(int x, int y)
        {
            Button_Down = false;
        }
        void nextEvent(XEvent &event)
        {
        //XSync(display, False);
        clearmouseclick(WM_LBUTTONDOWN);
        clearmouseclick(WM_LBUTTONUP);

        while(1)
        {
            ButtonPress=0;
            ButtonRelease=0;
            MotionNotify=0;
            KeyPress =0;
            clearmouseclick(WM_MOUSEMOVE);
            if(ismouseclick(WM_LBUTTONDOWN))
            {
                ButtonPress=1;
                event.type =ButtonPress;
                getmouseclick(WM_LBUTTONDOWN,event.xbutton.x,event.xbutton.y);
                event.xmotion.x=event.xbutton.x;
                event.xmotion.y=event.xbutton.y;
                Button_Down=true; //true
            }
            if(ismouseclick(WM_LBUTTONUP))
            {

                Button_Down=false; //false
                ButtonRelease=2;
                event.type= ButtonRelease;
                getmouseclick(WM_LBUTTONUP,event.xbutton.x,event.xbutton.y);
                event.xmotion.x=event.xbutton.x;
                event.xmotion.y=event.xbutton.y;
            }
            if(Button_Down && ismouseclick(WM_MOUSEMOVE))
                {
                MotionNotify=3;
                event.type= MotionNotify;
                getmouseclick(WM_MOUSEMOVE,event.xbutton.x,event.xbutton.y);
                event.xbutton.x=mousex();
                event.xbutton.y=mousey();
                registermousehandler(WM_LBUTTONUP, Up_handler);
                }

            if(kbhit())
            {
                chr=getch();
                KeyPress=4;
                event.type=KeyPress;
            }

            if(ButtonPress || ButtonRelease || MotionNotify || KeyPress)
                return;
        }
    }

      char charFromEvent(XEvent &event){

            char c;
            if(kbhit())
            {
                c=getch();
                return c;
            }
        return chr;
      }

      int flag=1;
      void echoKey(XEvent &event){

        char c = charFromEvent(event);
        string msg(1,c);
        int tw = textwidth(msg.c_str());
        if(flag==1)
        {
            event.xkey.x=0;
            event.xkey.y=0;
            flag=0;
        }
        drawText(event.xkey.x+tw/2, event.xkey.y, string(1,c));
        event.xkey.x +=tw;
      }


      bool checkEvent(XEvent &event){

            ButtonPress=0;
            ButtonRelease=0;
            MotionNotify=0;
            KeyPress=0;
            clearmouseclick(WM_MOUSEMOVE);
            if(ismouseclick(WM_LBUTTONDOWN))
            {
                ButtonPress=1;
                event.type =ButtonPress;
                getmouseclick(WM_LBUTTONDOWN,event.xbutton.x,event.xbutton.y);
                event.xmotion.x=event.xbutton.x;
                event.xmotion.y=event.xbutton.y;
                Button_Down=true; //true
            }
            if(ismouseclick(WM_LBUTTONUP))
            {

                Button_Down=false; //false
                ButtonRelease=2;
                event.type= ButtonRelease;
                getmouseclick(WM_LBUTTONUP,event.xbutton.x,event.xbutton.y);
                event.xmotion.x=event.xbutton.x;
                event.xmotion.y=event.xbutton.y;
            }
            if(Button_Down && ismouseclick(WM_MOUSEMOVE))
                {
                MotionNotify=3;
                event.type= MotionNotify;
                getmouseclick(WM_MOUSEMOVE,event.xbutton.x,event.xbutton.y);
                event.xbutton.x=mousex();
                event.xbutton.y=mousey();
                registermousehandler(WM_LBUTTONUP, Up_handler);
                }
            if(kbhit())
            {
                KeyPress=4;
                event.type=KeyPress;
                chr=getch();
            }
       if(ButtonPress || ButtonRelease || MotionNotify || KeyPress)
        return true;
       return false;
      }

      bool mouseDragEvent(XEvent &event){
        return event.type==MotionNotify;
      }

      bool mouseButtonPressEvent(XEvent &event){
        return event.type==ButtonPress;
      }


      bool mouseButtonReleaseEvent(XEvent &event){
        return event.type==ButtonRelease;
      }

      bool keyPressEvent(XEvent &event){

        return event.type == KeyPress;
      }

      int getClick(){

        XEvent event;
      
        clearmouseclick(WM_LBUTTONDOWN);
        clearmouseclick(WM_LBUTTONUP);
        clearmouseclick(WM_MOUSEMOVE);
        while(true){
          nextEvent(event);
          if(ButtonPress){
            return event.xbutton.x * 65536 + event.xbutton.y;
          }

        }
        return 0;
      }

    }
    Color COLOR(const char *color_string){
   
    if(screen_number==999){ 
          cout << "You must first call initCanvas before "<<
    	"using any graphics features.\n";
    	wait(5);
          exit(1);
        }

          if( strcmp(color_string, "red")==0)
              return ::COLOR(255,0,0);
          else if( strcmp(color_string, "green")==0)
              return ::COLOR(0,255,0);
          else if( strcmp(color_string, "blue")==0)
              return ::COLOR(0,0,255);
          else if( strcmp(color_string, "yellow")==0)
              return ::COLOR(255,255,0);
          else if( strcmp(color_string, "black")==0)
              return ::COLOR(0,0,0);
          else
              return ::COLOR(255,255,255);
   
      }

