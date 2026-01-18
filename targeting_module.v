module targeting_system(
    input wire clk,
    input wire rst_n, 
    input wire [2:0] sensor_in, 
    output reg proton_fire
);
  
  // State definitions
  localparam IDLE      = 3'd0;
  localparam CALIB_1   = 3'd1;
  localparam CALIB_2   = 3'd2;
  localparam GATE_LEFT = 3'd3;
  localparam WINDOW    = 3'd4;
  localparam FIRE      = 3'd5;
  
  reg [2:0] current_state;
  reg [4:0] cycle;
  reg [1:0] hits;
    
  always @(posedge clk or negedge rst_n) 
    begin
      if (!rst_n) 
        begin
          current_state <= IDLE;
          proton_fire   <= 0;
          cycle         <= 0;
          hits          <= 0;
        end
      else
        begin
          // Default: fire is 0 unless set to 1
          proton_fire <= 0; 
          
          case(current_state)
            
            IDLE: begin
              if(sensor_in == 3'b111)
                current_state <= CALIB_1;
              else
                current_state <= IDLE;
            end
                
            CALIB_1: begin
              if(sensor_in == 3'b111)
                current_state <= CALIB_2;
              else
                current_state <= IDLE;
            end
              
            CALIB_2: begin
              if(sensor_in == 3'b001)
                current_state <= GATE_LEFT;
              else if (sensor_in == 3'b010)
                current_state <= IDLE;
              else
                current_state <= IDLE;
            end
              
            GATE_LEFT: begin
              if(sensor_in == 3'b010)
                begin
                  current_state <= WINDOW;
                  cycle <= 1; 
                  hits  <= 0;
                end
              else 
                begin
                  current_state <= IDLE;
                end
            end
              
            WINDOW: begin
              if (sensor_in == 3'b101) 
                begin
                  current_state <= IDLE;
                end
              else if (sensor_in == 3'b100) 
                begin
                  if (hits == 1) 
                    begin
                      current_state <= FIRE;
                      proton_fire   <= 1; // Immediate fire
                    end
                  else 
                    begin
                      hits <= hits + 1;                          
                      if (cycle == 16)
                        current_state <= IDLE;
                      else
                        cycle <= cycle + 1;              
                    end
                end 
              else 
                begin
                  if (cycle == 16) 
                    current_state <= IDLE;
                  else 
                    cycle <= cycle + 1;
                end
            end
              
            FIRE: begin
              current_state <= IDLE;
            end
            
            default: current_state <= IDLE;
          endcase
        end
    end
endmodule
